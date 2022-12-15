#!/bin/bash

# Run each docker one at a time, hopefully exiting in the event of an error
services=$(grep '^\s*cpparpcalc' docker-compose.yml | sed 's/^\s*\(cpparpcalc.*\):/\1/')
for service in $services
do
	docker-compose up $service || exit 3
done
