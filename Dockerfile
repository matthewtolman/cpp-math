# Dockerfile to allow testing cross-building from zig build -Dtarget=x86_64-linux-musl
FROM alpine
COPY zig-out/bin/mt-maths-tests .
ENTRYPOINT ./mt-maths-tests
