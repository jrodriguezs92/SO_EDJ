# Use official ubuntu environment as a parent image

FROM jerodso/edj-ws-base

# Create directory for web service
RUN mkdir -p /usr/src/ws

# Set the working directory
WORKDIR /usr/src/ws

# Copy the current directory contents into the working directory
COPY . /usr/src/ws

# Compile the app
RUN make all

# Make port 8007 available
EXPOSE 8007
