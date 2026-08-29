# AOCS

Real-time attitude control. The loop runs on its own period and pulls its
sensor data rather than being invoked when a sample arrives. It relies on the
ping-pong buffer to always get the latest data.

![AOCS CSCI](../../docs/aocs_csci.svg)
