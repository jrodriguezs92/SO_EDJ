//********************************************************
//			Instituto Tecnológico de Costa Rica
//				Computer Engineering
//
//		Programmer: Esteban Agüero Pérez (estape11)
//		Programming Language: C
//		Version: 1.0
//		Last Update: 26/05/2019
//
//				Operating Systems Principles
//				Professor. Diego Vargas
//********************************************************

#include <linux/kernel.h>
#include <linux/errno.h>
#include <linux/init.h>
#include <linux/slab.h>
#include <linux/tty.h>
#include <linux/tty_driver.h>
#include <linux/tty_flip.h>
#include <linux/serial.h>
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/spinlock.h>
#include <asm/uaccess.h>
#include <linux/usb.h>
#include <linux/usb/serial.h>
#include <linux/sched/signal.h>

// Device IDs
#define VENDOR_ID		0x1A86
#define PRODUCT_ID		0x7523

// Buffer size
#define BUF_SIZE		1024

// Vendor definitions
#define VENDOR_WRITE_TYPE		0x40
#define VENDOR_READ_TYPE		0xC0
#define VENDOR_READ				0x95
#define VENDOR_WRITE			0x9A
#define VENDOR_SERIAL_INIT		0xA1
#define VENDOR_MODEM_OUT		0xA4
#define VENDOR_VERSION			0x5F

// UART Protocol
#define UART_CTS		0x01
#define UART_DSR		0x02
#define UART_RING		0x04
#define UART_DCD		0x08
#define CONTROL_OUT		0x10
#define CONTROL_DTR		0x20
#define	CONTROL_RTS		0x40
#define UART_STATE			0x00
#define UART_OVERRUN_ERROR	0x01
#define UART_PARITY_ERROR	0x02
#define UART_FRAME_ERROR	0x06
#define UART_RECV_ERROR		0x02
#define UART_STATE_TRANSIENT_MASK	0x07

// Port state
#define PORTA_STATE		0x01
#define PORTB_STATE		0x02
#define PORTC_STATE		0x03

static DECLARE_WAIT_QUEUE_HEAD(wq);
static int wait_flag = 0;

// Buffer Struct
struct arduino_buf {
	unsigned int buf_size;
	char *buf_buf;
	char *buf_get;
	char *buf_put;
};

// Control Struct 
struct arduino_private {
	spinlock_t	lock;	//access lock
	struct arduino_buf	*buf;
	int	write_urb_in_use; // USB Request Block
	unsigned baud_rate;
	wait_queue_head_t	delta_msr_wait;
	u8	line_control;
	u8	line_status;
	u8	termios_initialized;
};

// Supported device IDs
static struct usb_device_id	id_table [] = {
	{ USB_DEVICE(VENDOR_ID, PRODUCT_ID) },
	{ } // termination entry
};
MODULE_DEVICE_TABLE( usb, id_table );

// Allocates the buffer
// @author estape11
// @params size
// @return buffer
static struct arduino_buf *arduino_buf_alloc( unsigned int size ) {
	struct arduino_buf *pb;

	if( size == 0 ) {
		return NULL;

	}

	pb = kmalloc( sizeof(struct arduino_buf), GFP_KERNEL );
	if( pb == NULL ) {
		return NULL;

	}

	pb->buf_buf = kmalloc( size, GFP_KERNEL );

	if( pb->buf_buf == NULL ) {
		kfree(pb);
		return NULL;

	}

	pb->buf_size = size;
	pb->buf_get = pb->buf_put = pb->buf_buf;

	return pb;

}

// Release the buffer memory
// @author estape11
// @params buffer
// @return void
static void arduino_buf_free( struct arduino_buf *pb ) {
	if( pb ) {
		kfree( pb->buf_buf );
		kfree( pb );

	}

}

// Cleans the buffer
// @author estape11
// @params buffer
// @return void
static void arduino_buf_clear( struct arduino_buf *pb ) {
	if( pb != NULL ) {
		pb->buf_get = pb->buf_put;

	}

}

// Return the number of bytes of data available in the buffer
// @author estape11
// @params bufer
// @return dataBytes
static unsigned int arduino_buf_data_avail( struct arduino_buf *pb ) {
	if( pb == NULL ) {
		return 0;

	}

	return ((pb->buf_size + pb->buf_put - pb->buf_get) % pb->buf_size );
}

// Return the number of bytes of space available in the buffer
// @author estape11
// @params buffer
// @return spaceBytes
static unsigned int arduino_buf_space_avail( struct arduino_buf *pb ) {
	if( pb == NULL ) {
		return 0;

	}

	return ((pb->buf_size + pb->buf_get - pb->buf_put - 1) % pb->buf_size );
}

// Copy data from a user buffer and put it into the private buffer.
// @author estape11
// @params privateBuffer, buffer, count
// @return copiedBytes
static unsigned int arduino_buf_put( struct arduino_buf *pb,
		const char *buf, unsigned int count ) {
	unsigned int len;

	if( pb == NULL ) {
		return 0;

	}

	len = arduino_buf_space_avail(pb);
	if( count > len ) {
		count = len;

	} else if( count == 0 ) {
		return 0;

	}

	len = pb->buf_buf + pb->buf_size - pb->buf_put;
	if( count > len ) {
		memcpy( pb->buf_put, buf, len );
		memcpy( pb->buf_buf, buf+len, count - len );
		pb->buf_put = pb->buf_buf + count - len;

	} else {
		memcpy( pb->buf_put, buf, count );
		if( count < len ) {
			pb->buf_put += count;

		}
		else if( count == len ) {
			pb->buf_put = pb->buf_buf;

		}
	}

	return count;
}

// Copy data from private buffer  to a user buffer.
// @author estape11
// @params privateBuffer, buffer, count
// @return copiedBytes
static unsigned int arduino_buf_get( struct arduino_buf *pb, 
		char *buf, unsigned int count ) {

	unsigned int len;

	if( pb == NULL ) {
		return 0;

	}

	len = arduino_buf_data_avail(pb);
	if( count > len ) {
		count = len;

	} else if( count == 0 ) {
		return 0;

	}

	len = pb->buf_buf + pb->buf_size - pb->buf_get;
	if( count > len ) {
		memcpy( buf, pb->buf_get, len );
		memcpy( buf+len, pb->buf_buf, count - len );
		pb->buf_get = pb->buf_buf + count - len;

	}
	else {
		memcpy( buf, pb->buf_get, count );
		if( count < len ) {
			pb->buf_get += count;

		}
		else if( count == len ) {
			pb->buf_get = pb->buf_buf;

		}
	}

	return count;
}

// Vendor read control
// @author estape11
// @params request, index, serial, buffer
// @return retval
static int arduino_vendor_read( __u8 request,
		__u16 value,
		__u16 index,
		struct usb_serial *serial,
		unsigned char *buf,
		__u16 len ) {

	int retval;

	retval = usb_control_msg( serial->dev, usb_rcvctrlpipe(serial->dev, 0), 
			request, VENDOR_READ_TYPE, value, index, buf, len, 1000 );

	return retval;

}

// Vendor write control
// @author estape11
// @params request, index, serial, buffer
// @return retval
static int arduino_vendor_write( __u8 request,
		__u16 value,
		__u16 index,
		struct usb_serial *serial,
		unsigned char *buf,
		__u16 len ) {

	int retval;

	retval = usb_control_msg( serial->dev, 
			usb_sndctrlpipe(serial->dev, 0),
			request,
			VENDOR_WRITE_TYPE,
			value, index, buf, len, 1000 );

	return retval;
}

// Sets control lines
// @author estape11
// @params serial, value
// @return retval
static int set_control_lines( struct usb_serial *serial,
		u8 value ) {

	int retval;

	retval = arduino_vendor_write( VENDOR_MODEM_OUT, (unsigned short)value, 
			0x0000, serial, NULL, 0x00 );

	return retval;

}

// Calculates the baud rate
// @author estape11
// @params baud_rate, factor, divisor
// @return 0
static int arduino_get_baud_rate( unsigned int baud_rate,
		unsigned char *factor, unsigned char *divisor) {
	unsigned char a;
	unsigned char b;
	unsigned long c;

	switch ( baud_rate ) {
	case 921600:
		a = 0xf3;
		b = 7;
		break;
	case 307200:
		a = 0xd9;
		b = 7;
		break;
	default:
		if ( baud_rate > 6000000/255 ) {
			b = 3;
			c = 6000000;

		} else if ( baud_rate > 750000/255 ) {
			b = 2;
			c = 750000;

		} else if (baud_rate > 93750/255) {
			b = 1;
			c = 93750;

		} else {
			b = 0;
			c = 11719;

		}
		a = (unsigned char)(c / baud_rate);
		if (a == 0 || a == 0xFF) return -EINVAL;
		if ((c / a - baud_rate) > (baud_rate - c / (a + 1))) {
			a ++;

		}
		a = 256 - a;
		break;
	}
	*factor = a;
	*divisor = b;
	return 0;
}

// Set the termios values
// @author estape11
// @params tty, port, old_termios
// @return void
static void arduino_set_termios( struct tty_struct *tty,
		struct usb_serial_port *port, struct ktermios *old_termios ) {

	struct usb_serial *serial = port->serial;	
	struct arduino_private *priv = usb_get_serial_port_data(port);

	struct ktermios *termios = &tty->termios;

	unsigned int baud_rate;
	unsigned int cflag;
	unsigned long flags;
	u8 control;

	unsigned char divisor = 0;
	unsigned char reg_count = 0;
	unsigned char factor = 0;
	unsigned char reg_value = 0;
	unsigned short value = 0;
	unsigned short index = 0;
	
	spin_lock_irqsave( &priv->lock, flags );
	if( !priv->termios_initialized ) {
		*(termios) = tty_std_termios;
		termios->c_cflag = B9600 | CS8 | CREAD | HUPCL | CLOCAL;
		termios->c_ispeed = 9600;
		termios->c_ospeed = 9600;
		priv->termios_initialized = 1;
	}
	spin_unlock_irqrestore( &priv->lock, flags );

	/* 
	 * Note:
	 * The arduino is reported to lose bytes if you change serial setting
	 * even to the same vaules as before. Thus we actually need to filter 
	 * in this specific case.
	 */
	if( !tty_termios_hw_change(termios, old_termios) ) {
		return;

	}

	cflag = termios->c_cflag;

	// Get the byte size
	switch( cflag & CSIZE ) {
		case CS5:
			reg_value |= 0x00;
			break;
		case CS6:
			reg_value |= 0x01;
			break;
		case CS7:
			reg_value |= 0x02;
			break;
		case CS8:
			reg_value |= 0x03;
			break;
		default:
			reg_value |= 0x03;
			break;
	}

	// Figure out the stop bits 
	if( cflag & CSTOPB ) {
		reg_value |= 0x04;
		// stop bits = 2
	} // stop bits = 1

	// Determine the parity
	if (cflag & PARENB) {
		if (cflag & CMSPAR) {
			if (cflag & PARODD) {
    			reg_value |= (0x28 | 0x00);

			} else {
    			reg_value |= (0x38 | 0x10);

			}
		} else {
			if (cflag & PARODD) {
    			reg_value |= (0x08 | 0x00);
    			// parity =  odd
			} else {
    			reg_value |= (0x18 | 0x10);
    			// parity = even
			}
		}
	} // parity = none

	// Determine the baud rate
	baud_rate = tty_get_baud_rate( tty );
	arduino_get_baud_rate( baud_rate, &factor, &divisor );	
	
	// Enable SFR_UART RX and TX
	reg_value |= 0xc0;

	// Enable SFR_UART Control register and timer
	reg_count |= 0x9c;

	value |= reg_count;
	value |= (unsigned short)reg_value << 8;
	index |= 0x80 | divisor;
	index |= (unsigned short)factor << 8;
	arduino_vendor_write( VENDOR_SERIAL_INIT, value, index, serial, NULL, 0 );

	// Change control lines if we are switching to or from B0
	spin_lock_irqsave( &priv->lock, flags ); 
	control = priv->line_control;
	if( (cflag & CBAUD) == B0 ) {
		priv->line_control &= ~(CONTROL_DTR | CONTROL_RTS);

	} else {
		priv->line_control |= (CONTROL_DTR | CONTROL_RTS);

	}

	if( control != priv->line_control ) {
		control = priv->line_control;
		spin_unlock_irqrestore( &priv->lock, flags );
		set_control_lines( serial, control );

	} else {
		spin_unlock_irqrestore( &priv->lock, flags );

	}

	if( cflag & CRTSCTS ) {
		arduino_vendor_write( VENDOR_WRITE, 0x2727, 0x0101, serial, NULL, 0);

	}

	// FIXME: Need to read back resulting baud rate
	if(  baud_rate ) {
		tty_encode_baud_rate(tty, baud_rate, baud_rate);

	}

}

// Obtain the modem status bits from the tty driver
// @author estape11
// @params tty
// @return retval
static int arduino_tiocmget( struct tty_struct *tty ) {
	struct usb_serial_port *port = tty->driver_data;
	struct arduino_private *priv = usb_get_serial_port_data(port);
	unsigned long flags;
	unsigned int mcr;
	unsigned int retval;

	if( !usb_get_intfdata( port->serial->interface) ) 
		return -ENODEV;

	spin_lock_irqsave( &priv->lock, flags );
	mcr = priv->line_control;
	spin_unlock_irqrestore( &priv->lock, flags );

	retval = ((mcr & CONTROL_DTR) ? TIOCM_DTR : 0) |
			 ((mcr & CONTROL_RTS) ? TIOCM_RTS : 0) |
			 ((mcr & UART_CTS) ? TIOCM_CTS : 0) |
			 ((mcr & UART_DSR) ? TIOCM_DSR : 0) |
			 ((mcr & UART_RING) ? TIOCM_RI : 0) |
			 ((mcr & UART_DCD) ? TIOCM_CD : 0);

	return retval;
}

// Close a device
// @author estape11
// @params port
// @return void
static void arduino_close( struct usb_serial_port *port ) {
	struct tty_struct *tty = port->port.tty;
	struct arduino_private *priv = usb_get_serial_port_data(port);
	unsigned long flags;
	unsigned int c_cflag;

	printk(KERN_INFO "<*> arduino-meArm: device closed");

	spin_lock_irqsave( &priv->lock, flags );
	// clear out any remaining data in the buffer
	arduino_buf_clear( priv->buf );
	spin_unlock_irqrestore( &priv->lock, flags );


	// shutdown our urbs
	usb_kill_urb(port->interrupt_in_urb);
	usb_kill_urb(port->read_urb);
	usb_kill_urb(port->write_urb);
	//usb_serial_generic_close(port, filp);

	if( tty ) {
		c_cflag = tty->termios.c_cflag;

		if( c_cflag & HUPCL ) {
			// drop DTR and RTS
			spin_lock_irqsave( &priv->lock, flags );
			priv->line_control = 0;
			spin_unlock_irqrestore( &priv->lock, flags );
			set_control_lines( port->serial, 0 );

		}
	}
}

// Open a device
// @author estape11
// @params tty, port
// @return retval
static int arduino_open( struct tty_struct *tty,
		struct usb_serial_port *port ) {
	struct ktermios tmp_termios;
	struct usb_serial *serial = port->serial;
	int retval;

	usb_clear_halt( serial->dev, port->write_urb->pipe );
	usb_clear_halt( serial->dev, port->read_urb->pipe );

	if( tty ) {
		arduino_set_termios( tty, port, &tmp_termios );

	}

	port->read_urb->dev = serial->dev; 
	retval = usb_submit_urb( port->read_urb, GFP_KERNEL );
	if(retval) {
		printk(KERN_INFO "<*> arduino-meArm: failed submit read");
		arduino_close(port);
		goto err_out;

	}
	port->interrupt_in_urb->dev = serial->dev; 
	retval = usb_submit_urb( port->interrupt_in_urb, GFP_KERNEL );
	if(retval) {
		printk(KERN_INFO "<*> arduino-meArm: failed submit read");
		arduino_close(port);
		goto err_out;

	}
	printk(KERN_INFO "<*> arduino-meArm: device open\n");

err_out:
	return retval;
}

// Set the modem status bits for the tty driver
// @author estape11
// @params tty, set, clear
// @return retval
static int arduino_tiocmset( struct tty_struct *tty,
		unsigned int set, unsigned int clear ) {
	struct usb_serial_port *port = tty->driver_data;

	struct arduino_private *priv = usb_get_serial_port_data(port);
	unsigned long flags;
	/*unsigned int mcr = priv->line_control;*/
	u8 control;

	if( !usb_get_intfdata(port->serial->interface) ) {
		return -ENODEV;

	}

	spin_lock_irqsave( &priv->lock, flags );
	if( set & TIOCM_RTS ) {
		priv->line_control |= CONTROL_RTS;

	}
	if( set & TIOCM_DTR ) {
		priv->line_control |= CONTROL_DTR;

	}
	if( clear & TIOCM_RTS ) {
		priv->line_control &= ~CONTROL_RTS;

	}
	if( clear & TIOCM_DTR ) {
		priv->line_control &= ~CONTROL_DTR;

	}
	control = priv->line_control;
	spin_unlock_irqrestore( &priv->lock, flags );

	return set_control_lines( port->serial, control );
}

// Obtain the modem info
// @author estape11
// @params port, arg
// @return 0
static int wait_modem_info( struct usb_serial_port *port,
		unsigned int arg ) {
	struct arduino_private *priv = usb_get_serial_port_data(port);
	unsigned long flags;
	unsigned int prevstatus;
	unsigned int status;
	unsigned int changed;

	spin_lock_irqsave( &priv->lock, flags );
	prevstatus = priv->line_status;
	spin_unlock_irqrestore( &priv->lock, flags );

	while(1) {
		wait_event_interruptible( wq, wait_flag != 0 );
		wait_flag = 0;
		// see if a signal did it
		if( signal_pending(current) ) {
			return -ERESTARTSYS;

		}

		spin_lock_irqsave( &priv->lock, flags );
		status = priv->line_status;
		spin_unlock_irqrestore( &priv->lock, flags );

		changed = prevstatus ^ status;

		if( ((arg & TIOCM_RNG) && (changed & UART_RING)) ||
			((arg & TIOCM_DSR) && (changed & UART_DSR))  ||
			((arg & TIOCM_CD)  && (changed & UART_DCD))  ||
			((arg & TIOCM_CTS) && (changed & UART_CTS)) )
			return 0;

		prevstatus = status;
	}

	// Not reatched
	return 0;
}

// Obtain the modem info
// @author estape11
// @params port, arg
// @return err
static int arduino_ioctl( struct tty_struct *tty,
		unsigned int cmd, unsigned long arg ) {
	struct usb_serial_port *port = tty->driver_data;

	switch(cmd) {
		case TIOCMIWAIT:
			return wait_modem_info(port, arg);

		default:
			break;
	}

	return -ENOIOCTLCMD;
}

// Send data to a device
// @author estape11
// @params port
// @return 0
static void arduino_send( struct usb_serial_port *port ) {
	int count;
	int retval;
	struct arduino_private *priv = usb_get_serial_port_data( port );
	unsigned long flags;

	spin_lock_irqsave( &priv->lock, flags );
	if( priv->write_urb_in_use ) {
		spin_unlock_irqrestore( &priv->lock, flags );
		return;

	}

	count = arduino_buf_get( priv->buf, port->write_urb->transfer_buffer,
			port->bulk_out_size );
	if( count == 0 ) {
		spin_unlock_irqrestore( &priv->lock, flags );
		return;

	}

	//printk("<*> arduino-meArm: writting -> %s \n\n", port->write_urb->transfer_buffer);
	printk("<*> arduino-meArm: writting data");


	priv->write_urb_in_use = 1;
	spin_unlock_irqrestore( &priv->lock, flags );

	usb_serial_debug_data( &port->dev, __func__, count,
			port->write_urb->transfer_buffer );

	port->write_urb->transfer_buffer_length = count;
	port->write_urb->dev = port->serial->dev;
	retval = usb_submit_urb( port->write_urb, GFP_ATOMIC );
	if( retval ) {
		printk(KERN_INFO "<*> arduino-meArm: failed submitting write");
		priv->write_urb_in_use = 0;
		// reschedule arduino_send
	}

	usb_serial_port_softint( port ); 
}

// Write data to a device
// @author estape11
// @params tty, port, buffer, count
// @return count
static int arduino_write( struct tty_struct *tty, 
		struct usb_serial_port *port, const unsigned char *buf, int count ) {
	struct arduino_private *priv = usb_get_serial_port_data(port);
	unsigned long flags;

	if( !count ) {
		return count;

	}

	spin_lock_irqsave( &priv->lock, flags );
	count = arduino_buf_put( priv->buf, buf, count ); 
	spin_unlock_irqrestore( &priv->lock, flags );

	arduino_send(port);

	return count;
}

// Gets how much room in the write buffer the tty driver has available
// @author estape11
// @params tty
// @return count
static int arduino_write_room( struct tty_struct *tty ) {
	struct usb_serial_port *port = tty->driver_data;
	struct arduino_private *priv = usb_get_serial_port_data( port );
	int room = 0;
	unsigned long flags;
	spin_lock_irqsave( &priv->lock, flags );
	room = arduino_buf_space_avail( priv->buf );
	spin_unlock_irqrestore( &priv->lock, flags );
	return room;

}

// Gets how many characters are still remaining in the tty driver's write buffer to be sent out
// @author estape11
// @params tty
// @return chars
static int arduino_chars_in_buffer( struct tty_struct *tty ) {
	struct usb_serial_port *port = tty->driver_data;
	struct arduino_private *priv = usb_get_serial_port_data(port);
	int chars = 0;
	unsigned long flags;
	spin_lock_irqsave( &priv->lock, flags );
	chars = arduino_buf_data_avail( priv->buf );
	spin_unlock_irqrestore( &priv->lock, flags );
	return chars;

}

// Register with the kernel driver core the device
// @author estape11
// @params serial
// @return err
static int arduino_attach( struct usb_serial *serial ) {
	struct arduino_private *priv; 
	int i;
	char buf[8];

	printk(KERN_INFO "<*> arduino-meArm: new device attached");

	for( i = 0; i < serial->num_ports; ++i ) {
		priv = kzalloc( sizeof(struct arduino_private), GFP_KERNEL );
		if( !priv ) {
			goto cleanup;

		}
		spin_lock_init( &priv->lock );
		priv->buf = arduino_buf_alloc( BUF_SIZE );
		if( priv->buf == NULL ) {
			kfree( priv );
			goto cleanup;

		}
		init_waitqueue_head( &priv->delta_msr_wait );
		usb_set_serial_port_data( serial->port[i], priv );

	}

	arduino_vendor_read( VENDOR_VERSION, 0x0000, 0x0000,
			serial, buf, 0x02 );
	arduino_vendor_write( VENDOR_SERIAL_INIT, 0x0000, 0x0000,
			serial, NULL, 0x00 );
	arduino_vendor_write( VENDOR_WRITE, 0x1312, 0xD982, 
			serial, NULL, 0x00 );
	arduino_vendor_write( VENDOR_WRITE, 0x0F2C, 0x0004,
			serial, NULL, 0x00 );
	arduino_vendor_read( VENDOR_READ, 0x2518, 0x0000,
			serial, buf, 0x02 );
	arduino_vendor_write( VENDOR_WRITE, 0x2727, 0x0000,
			serial, NULL, 0x00 );
	arduino_vendor_write( VENDOR_MODEM_OUT, 0x009F, 0x0000,
			serial, NULL, 0x00 );

	return 0;

cleanup:
	for( --i; i >= 0; --i ) {
		priv = usb_get_serial_port_data( serial->port[i] );
		arduino_buf_free( priv->buf );
		kfree( priv );
		usb_set_serial_port_data( serial->port[i], NULL );
	}

	return -ENOMEM;
}

// Updates the UART line status
// @author estape11
// @params port, data, len
// @return void
static void arduino_update_line_status( struct usb_serial_port *port,
		unsigned char *data, unsigned int actual_length ) {
	struct arduino_private *priv = usb_get_serial_port_data( port );
	unsigned long flags;
	u8 length = UART_STATE + 0x04;

	if( actual_length < length )
		return;

	// Save off the uart status for others to look at
	spin_lock_irqsave( &priv->lock, flags );
	priv->line_status = data[UART_STATE];
	priv->line_control = data[PORTB_STATE];
	spin_unlock_irqrestore( &priv->lock, flags );
	wait_flag = 1;
	wake_up_interruptible( &priv->delta_msr_wait );

}

// Interruption callback
// @author estape11
// @params urb
// @return void
static void arduino_read_int_callback( struct urb *urb ) {
	struct usb_serial_port *port = (struct usb_serial_port *)urb->context; 
	unsigned char *data = urb->transfer_buffer;
	unsigned int actual_length = urb->actual_length;
	int status = urb->status;
	int retval;

	switch( status ) {
		case 0: //success
			break;
		case -ECONNRESET:
		case -ENOENT:
		case -ESHUTDOWN: //this urb is terminated, clean up
			printk(KERN_INFO "<*> arduino-meArm: shutting down");
			return;
		default:
			printk(KERN_INFO "<*> arduino-meArm: nonzero urb status received");
			goto exit;
	}
	usb_serial_debug_data( &port->dev, __func__,
			urb->actual_length, urb->transfer_buffer );

	arduino_update_line_status( port, data, actual_length );

exit:
	retval = usb_submit_urb( urb, GFP_ATOMIC ); 
	if( retval ) {
		printk(KERN_INFO "<*> arduino-meArm: usb_submit_urb failed");

	}

}

// Initial callbacks for these kinds of USB endpoints
// @author estape11
// @params urb
// @return void
static void arduino_read_bulk_callback( struct urb *urb ) {
	struct usb_serial_port *port = (struct usb_serial_port *)urb->context;
	struct arduino_private *priv = usb_get_serial_port_data( port );
	struct tty_struct *tty;
	unsigned char *data = urb->transfer_buffer;
	unsigned long flags;
	int i;
	int retval;
	int status = urb->status;
	u8 line_status;
	char tty_flag;

	if( status ) {
		if( status == -EPROTO ) {
			printk(KERN_INFO "<*> arduino-meArm: caught -EPROTO, resubmitting the urb");
			urb->dev = port->serial->dev;
			retval = usb_submit_urb( urb, GFP_ATOMIC );
			if( retval ) {
				printk(KERN_INFO "<*> arduino-meArm: failed resubmitting read urb");
				return;

			}
		}

		printk(KERN_INFO "<*> arduino-meArm: exiting");
		return;

	}

	usb_serial_debug_data( &port->dev, __func__,
			urb->actual_length, data );

	// get tty_flag from status
	tty_flag = TTY_NORMAL;

	spin_lock_irqsave( &priv->lock, flags );
	line_status = priv->line_status;
	priv->line_status &= ~UART_STATE_TRANSIENT_MASK;
	spin_unlock_irqrestore( &priv->lock, flags );
	wait_flag = 1;  
	wake_up_interruptible( &priv->delta_msr_wait );

	// break takes precedence over parity, 
	// which takes precedence over framing errors
	if( line_status & UART_PARITY_ERROR ) {
		tty_flag = TTY_PARITY;

	} else if( line_status & UART_OVERRUN_ERROR ) {
		tty_flag = TTY_OVERRUN;

	} else if( line_status & UART_FRAME_ERROR ) {
		tty_flag = TTY_FRAME;

	}

	tty = port->port.tty; 

	if( tty && urb->actual_length ) {

		tty_buffer_request_room( tty->port, urb->actual_length + 1);

		// overrun is special, not associated with a char
		if( line_status & UART_OVERRUN_ERROR ) {
			tty_insert_flip_char( tty->port, 0, TTY_OVERRUN );

		}


		for( i = 0; i < urb->actual_length; ++i ) {
			tty_insert_flip_char( tty->port, data[i], tty_flag );

		}

		tty_flip_buffer_push( tty->port );

	}

	//Schedule the next read _if_ we are still open
	urb->dev = port->serial->dev;
	retval = usb_submit_urb( urb, GFP_ATOMIC );
	if( retval ) {
		printk(KERN_INFO "<*> arduino-meArm: failed resubmitting read urb");

	}

	return;
}

// Initial callbacks for these kinds of USB endpoints
// @author estape11
// @params urb
// @return void
static void arduino_write_bulk_callback( struct urb *urb ) {
	struct usb_serial_port *port = (struct usb_serial_port *)urb->context;
	struct arduino_private *priv = usb_get_serial_port_data(port);
	int retval;
	int status = urb->status;

	switch( status ) {
		case 0: //success
			break;
		case -ECONNRESET:
		case -ENOENT:
		case -ESHUTDOWN:
			// this urb is terminated, clean up
			printk(KERN_INFO "<*> arduino-meArm: urb shutting down");
			priv->write_urb_in_use = 0;
			return;
		default:
			// error in the urb, so we have to resubmit it
			printk(KERN_INFO "<*> arduino-meArm: Overflow in write");
			printk(KERN_INFO "<*> arduino-meArm: nonzero write bulk status received");
			port->write_urb->transfer_buffer_length = 1;
			port->write_urb->dev = port->serial->dev;
			retval = usb_submit_urb(port->write_urb, GFP_ATOMIC);
			if( retval ) {
				printk(KERN_INFO "<*> arduino-meArm: failed resubmitting write urb");

			}

			else 
				return;
	}

	priv->write_urb_in_use = 0;

	// send any buffered data
	arduino_send(port);
}

// Definition of a usb driver
static struct usb_serial_driver	arduino_device = {
	.driver = {
		.owner	= THIS_MODULE,
		.name	= "arduino-meArm",
	},
	.id_table	= id_table,
	.num_ports		= 1,
	.open			= arduino_open,
	.close			= arduino_close,
	.write			= arduino_write,
	.ioctl			= arduino_ioctl,
	.set_termios	= arduino_set_termios,
	.tiocmget		= arduino_tiocmget,
	.tiocmset		= arduino_tiocmset,
	.read_bulk_callback  = arduino_read_bulk_callback,
	.read_int_callback   = arduino_read_int_callback,
	.write_bulk_callback = arduino_write_bulk_callback,
	.write_room     = arduino_write_room,
	.chars_in_buffer = arduino_chars_in_buffer,
	.attach			= arduino_attach,
	// termination entry
};

static struct usb_serial_driver *const serial_driver [] = {
	&arduino_device, NULL
};

static int __init arduino_init(void) {
	printk(KERN_INFO "<*> arduino-meArm: registering driver with kernel\n");
	return usb_serial_register_drivers( serial_driver, 
			KBUILD_MODNAME, id_table );

}

static void __exit arduino_exit(void) {
	printk(KERN_INFO "<*> arduino-meArm: unregistering driver with kernel\n");
	usb_serial_deregister_drivers( serial_driver );

}

module_init( arduino_init );
module_exit( arduino_exit );

MODULE_DESCRIPTION("Simple Arduino UNO driver for meArm");
MODULE_AUTHOR("estape11");
MODULE_LICENSE("GPL");