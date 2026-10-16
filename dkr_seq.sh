#!/bin/bash

# Run each docker one at a time, hopefully exiting in the event of an error
services=$(grep '^\s*cpparpcalc' docker-compose.yml | sed 's/^\s*\(cpparpcalc.*\):/\1/')
for service in $services
do
	# Without --exit-code-from, "up" succeeds even if the container fails
	docker compose up --exit-code-from $service $service || exit 3
done
