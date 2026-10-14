#!/bin/bash

# Run each docker one at a time, hopefully exiting in the event of an error
# (the static and Fedora Windows builds are only run when asked for)
services=$(grep '^\s*cpparpcalc' docker-compose.yml | grep -v -e '_static:' -e '_fedora:' | sed 's/^\s*\(cpparpcalc.*\):/\1/')
for service in $services
do
	docker-compose up $service || exit 3
done
