# Use official ubuntu environment as a parent image
FROM jrei/systemd-ubuntu:16.04

# Create directory for web service
RUN mkdir -p /usr/src/ws

# Set the working directory
WORKDIR /usr/src/ws

# Copy the current directory contents into the working directory
COPY . /usr/src/ws

# Update repository to install packages
RUN apt-get update

# Install build-essentia
RUN apt-get -y install build-essential

# Compile the app
RUN make all

# Make port 8001 available
EXPOSE 8001