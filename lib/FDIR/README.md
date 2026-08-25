# Fault Detection Interrupt Recovery (FDIR)







## Introduction and Purpose

FDIR module is a receiver of alarms and consumer of telemetry, it must decide the action to occur when alarms are raised. FDIR consists of an alarm queue that receives alarms from the Health Monitor (which may also decide to push things to other CSCIs or CSSCs. In other words, FDIR is the final authority over (in this demo) over AOCS (Rate Damping), but should also be responsible for power switching, payload shutdown, transitions to the Recovery mode State.

Since FDIR has a bounded queue of alarms, it must also publish its own Telemetry, such as the number of dropped alarms. Alarms shouldn't be dropped, so this is certainly a shotcoming of the design decisions I've taken.