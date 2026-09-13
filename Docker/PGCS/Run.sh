#!/bin/bash

# Run script for PGCS Docker container
# This script starts the PGCS process and monitors traffic

# Start supervisor
/usr/bin/supervisord -c /home/ng/workspace/novagenesis/supervisord.conf &

# Wait a bit for services to start
sleep 5

# Monitor traffic with tcpdump (as suggested by user)
# Use: tcpdump -i <interface> -nn -XX 'ether proto 0x1234'
echo "PGCS container started. Use tcpdump to monitor traffic:"
echo "tcpdump -i eth0 -nn -XX 'ether proto 0x1234'"

# Keep container running
tail -f /var/log/supervisor/supervisord.log
