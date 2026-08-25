# On-Board Software Architecture
## Problem Statement Brief
The OBC of an upcoming mission has a flight-ready embedded processor, compatible with Linux and FreeRTos. It is tasked with:
1. Scheduling of commands and payload operations
2. Real-time AOCS control
3. Receiving ground commands and routing commands to subsystems
4. Monitoring system health and triggering recovery
5. Buffering payload data until downlink

Create a high-level architecture plan for the Flight Software (FSW) stack that will run on this OBC. Present the resources you need or assume, such as for example I/O interfaces, memory and CPU head-room.
Clear diagrams and architectural explanations are encouraged.

## State

### Demo Presence
| # | Task | CSCI | Component (under /lib), demo status |
|---|---|---|---|
| 1 | Scheduling of Commands & Payload Operations | Scheduling CSCI | N/A, not implemented
| 2 | AOCS control | AOCS CSCI| AOCS/Rate Damper, Implemented
| 3 | Reception and Routing | TTC CSCI | N/A, not implemented
| 4 | Health Monitoring | Health Monitoring CSCI | HMS, implemented
| 5 | Buffering payload data | DataBase CSCI | Databases/PingPongBuffer, Databases/TelemetryDB, Implemented

Demo coded in C++ using C++23 - as I am using it in my home projects with a fully self built build-system. I prioritise the use of self-made algorithms rather than Commercial off the Shelf (CotS) as I am prrioritising my learning/improvement in design and coding.

## Assumptions
| # | Assumption & Constraints | Rationale/Justification |
|---|---|---|
| 1 | The system runs both Linux and FreeRTOS. | Currently I am working on a SoC that has cores isolated to run Embedded Linux and baremetal, safety critical items run in the Platform, less-critical impose a little bit on the Embedded Linux side. |
| 2 | Linux Cores are shared with the payload | This is a common weight and cost reduction measure. |
| 3 | The device is a space-grade / radiation-tolerant part. | Memory integrity handling is a software responsibility; see `[ref]`. |
| 4 | **Power and Thermal budget out of scope.** | Unfortunately not an expert. |
| 5 | Single embedded processor indicates one SoC | This impacts redundancy as it is the ONLY processor available, fault tolerance must be answered in Software. |
| 6 | S-band ground link | 10 kbps - 10Mbps data throughput. |
| 7 | VLEO Orbit | Should have a 90 minute orbit time, with some orbits seeing multiple landing stations, and some 0 |
| 8 | Signal quality doesn't degrade | This is just to not dive into MODCODs and signal degredations, and reduce sources of error |
| 9 | Imagery mission | Telecommunication satellites have different buffering requirements (packets can be dropped based on a Time To Live). |
| 10 | Image rate | 50 MB per image, 100 images per hour (average). This helps estimate storage requirements (images alone generate 5GB/hour).  |
| 11 | Prioritise use of F prime | Self-imposed constraint, I thought it'd be fun. see `[caveat1]` |
| 12 | Reliability is prioritized | As much data as possible should be retained in order to transmit when landinig stations are next in view |

## Some comments
`[caveat1]` I am not extremely familiar with F Prime, Ethan had brought it up and I thought it was an excellent option as it gives CotS implementations (especially with CCSDS). I am making an assumption that it's qualified and up to standards set forth by CCSDS
`[note1]` SoC suggestion to contain PL would be to use a versal, it containing one R5 core (could be used to run RTOS), an A72 (embedded linux) and programmable logic (SpaceWire Comms/sensor reading)!

## Sources
`[s1]`
`[s2]`
`[s3]`
`[s4]`


## Domains
### Real-Time Domain
The real time domain is tasked with all the safety critical aspects - Scheduling of Commands and Payload operations, AOCS Control, Reception and Routing, Health Monitoring (Platform), this is to get the deterministic latency under a real-time operating system, and, isolate a full Linux userspace for the Payload. User-space linux applications would not be able to prevent the real-time aspects to starve or, in the event of having a corruption on a core, the payload should be isolated from the RTOS so as to prevent AOCS.

| CSCI | CSC | Description |
|---|---|---|
| Scheduling | - | Owns the time-based command schedule. The schedule consists of both validating packets on receipt, and publishing the time until the next scheduled command. Schedules must also contain the time at which the next downlink (or all downlinks). The payload mission plan and downlink schedule must be stored in a non-volatile, radiation resistant medium (MRAM) |
| - | CCSDS/PUS Dispatcher | Dispatching is the notion of sendinding things out, I have just decided to also add the right time to this. The Dispatcher is required to also schedule  Telecommands (TC). A decision has been taken to use CCSDS PUS as their APIDs map easily to CSCIs. Requests for on-demand telemetry are also treated as Telecommands, to minimize the need for other dispatching mechanisms. |
| - | Downlink Scheduler | This owns the data transmissions, data is categorized in terms of criticality (1/2/3/4) see `[criticality ]` and drains the buffered data accordingly. Transmitting from the buffered platform and payload data. |
| Telemetry and Telecommand (TTC) | - | Owns the connection with the ground. This CSCI corresponds to the transport layer. |
| - | SpaceWire | Carries the TTC transfer frames to/from the communications subsystem. |
| - | Encryption | Suggested for completion's sake. People have demonstrated the ability to already hack satellites, AES-256 is a supported encryption via CCSDS SDLS. |
| Database | - | This CSCI is split across domains, and is responsible for maintaining the integrity of the database. |
| - | Data Buffer | Data Buffer exists to store all the different criticalities of data see `[criticality ]`. The most critical data is stored in non-volatile, radiation resistance storage (MRAM). |
| - | Parameter Database | Table of the current per-parameter latest. This may be stored in DDR with ECC. All data reads pass through this object to standardize interactions with the database in order to ensure reads/writes, timestamping and validity are enforced here rather than the telemetry producer. |
| - | Configurations | Configuration parameters of sensors, devices, limits etc. Separated from the Parameter Database as these should only be updated via ground operator commands or updates. |
| - | Compression | Assumption of the S-Band antenna with my data-throughput estimations requires the use of compression to meet the throughputs possible during the times we see a landing station. see `[compression]` |
| - | Metadata | Storage location/object of checksums for payload data, allowing us to still get some smaller bits of data through when we may not be able to transfer a whole image. |
| Health Monitoring (Platform) | - | Responsible for raising alarms, and ensuring telemetry within required ranges. |
| - | watchdog | Exists to kick an external watchdog. |
| - | Health Monitor | A very creative name, it exists to centralize the logic to monitor different telemetries. |
| Fault Detection and Interrupt Recovery | - | Final authority on spacecraft operations. Owns the spacecraft mode and is the centralized location for state transitions. |
| - | Boot | Responsible for monitoring the linux cores during boot. |
| - | State | The state machine for the satellite see `[platform states]`. |
| - | Power and Payload Control | Centralized location for power switching and shutting down the payload for recovery operations. |


### Linux Domain
The linux domain is deliberately set to be simple and lightweight (and out of scope).
| # | CSCI | Description |
|---|---|---|
| 1 | Payload Software | | Produces Telemetry (TM), Receives TCs from TTC `[TTC]` |
| 2 | Health Monitoring (Payload) | Separate from the Health Monitoring (Platform), this provides an easy way to separate payload by criticality, while reliability is prioritized, not all telemetry is equal |

 It contains the Payload Software CSCI - which acts a receiver for buffered payload operations. Payload software also produces Telemetry (TM).