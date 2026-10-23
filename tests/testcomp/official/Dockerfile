# Test-Comp 2026 reproduction environment.
#
# The base is the image the competition publishes as "mostly the same packages
# as the competition machines" (Ubuntu 24.04; competition-scripts
# test/Dockerfile.user.2026). On top of it, the two components the
# testcomp26 tag of gitlab.com/sosy-lab/test-comp/bench-defs pins:
#   - BenchExec 3.34 (submodule benchexec @ 829bf7a4583e), which runs both the
#     test generator and the validator under the official limits;
#   - TestCov 3.11.post3 (fm-tools testcov.yml, Zenodo 10.5281/zenodo.17698774)
#     and TestCoCa testcomp26 (fm-tools testcoca.yml, Zenodo
#     10.5281/zenodo.17819536): the five validators of the category structure
#     (TestCov x {gcc, clang} x {formatted, unformatted}, plus TestCoCa). A
#     task's score is the best coverage any of them measures.
# See docs/reports/2026-10-06-ambiente-testcomp-oficial.md.
#
# Run it privileged, as BenchExec needs to manage cgroups:
#   docker run --privileged --cap-drop=all ... map2check-testcomp-env <cmd>
FROM registry.gitlab.com/sosy-lab/benchmarking/competition-scripts/user@sha256:2a38216dfc6ceb6ebbb80f07dae8abd4b87b4c619d7c6db2256dc98e608ebc47

RUN apt-get update \
 && apt-get install -y --no-install-recommends python3-pip zip unzip xmlstarlet ca-certificates curl fuse-overlayfs \
 && rm -rf /var/lib/apt/lists/*

RUN pip install --break-system-packages --no-cache-dir benchexec==3.34

ARG TESTCOV_URL=https://zenodo.org/api/records/17698774/files/testcov-3.11.post3.zip/content
ARG TESTCOV_SHA256=604c234cb2ef6f8aa46bc0dd8fa146104d7d368b7285d0d8e9cd8bc99a7813b1
RUN curl -sSL -o /tmp/testcov.zip "$TESTCOV_URL" \
 && echo "$TESTCOV_SHA256  /tmp/testcov.zip" | sha256sum -c - \
 && unzip -q /tmp/testcov.zip -d /opt \
 && rm /tmp/testcov.zip \
 && /opt/testcov/bin/testcov --version

ARG TESTCOCA_URL=https://zenodo.org/api/records/17819536/files/testcoca.zip/content
ARG TESTCOCA_SHA256=14e57b70ee8ccb72899daa14ec7132cca217d8c334bf8b29940ca703ff32036f
RUN curl -sSL -o /tmp/testcoca.zip "$TESTCOCA_URL" \
 && echo "$TESTCOCA_SHA256  /tmp/testcoca.zip" | sha256sum -c - \
 && unzip -q /tmp/testcoca.zip -d /opt \
 && rm /tmp/testcoca.zip

COPY init.sh /init.sh
ENTRYPOINT ["/init.sh"]
CMD ["bash"]
