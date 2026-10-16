# Runs the calculator core's tests (tests/run_tests.sh and tests/run_fuzz.sh),
# so nothing needs installing on the host: see "Tests" in README.md.
FROM ubuntu:24.04

RUN apt-get update && apt-get install -y --no-install-recommends \
		g++ \
		libgmp-dev \
		libmpfr-dev \
		python3 \
		python3-venv \
	&& rm -rf /var/lib/apt/lists/*

# For the conversion test (tests/generate_conversion_tests.py)
RUN python3 -m venv /opt/pyenv \
	&& /opt/pyenv/bin/pip install --no-cache-dir "pint==0.26.1"
ENV PATH=/opt/pyenv/bin:${PATH}

WORKDIR /src
CMD ["bash", "tests/docker_tests.sh"]
