# Code Deployment and Considerations
## Problem
Present the approach you would take to integrate your proof - of- concept code in
the flight-software (FSW). Given that you would oversee the FSW, what other steps
would be required to bring it to a flight -ready status ? Which additional real-world
considerations would you have to ma ke that you have not already covered in
this task? How would your approach be changed to fit an FPGA - based OBC?

---

## 1. Approach to integrate the POC into the FSW
1. Create Interface Control document and ensure to clearly identify:
   inputs, outputs, units, timing budget, and error cases. Ensure that configurations may be dependency injected so the same code can run vs a simulated environment.

2. Ensure the use of the heap is at a minimum. Dynamic allocation of memory deadens the lifespan of memory.

3. Ensure adherance to the architecture decisions instructed by the README (use sensor front end, add in all CSCIs). Also ensure that the Operational state machine is implemented in addition to linking that with the FDIR module.

4. Make items configuration, allow for gains, threshholds for alarm tripping, task rates and buffer sized to be configurable based on concrete, real-world needs.

---

## 2. Reaching Flight Ready Status

### Process and standards
- Discuss, and select a flight coding standard.
- Determine Softeware Criticality classification. Safety critical aspects of platforms are often subject to different code coverage, test and implementation rules.
- Ensure valid, testable requirements are created for the Platform's Software. Ensure that code, and tests are traced to each other, and both traced to requirements.
- Documentation: ensure Software Requirement Specifications, Software High Level and Detailed Design, Interface Control Documents, Software Verification and Version Description documents are created and reviewed.
- Formal review gates: PDR, CDR, TRR, software qualification review, FRR.

### Verification and validation
- Stringent static analysis and ensure the whole SW stack abides by best practices.
- Analyse timing for Worst Case Error Timing for each task running in the RTOS domain. Similarly, determine headroom and memory budget versus the 50% design-stage goal.
- Testing: fault injections, forced error conditions, functional testing. Attempt to model the FDIR system vs all the possible errors and prolonged occurrences.
- Independent Software V&V (IVV) for the critical items.
- Validate the numbers I estimated are valid.

---

## 3. How the approach changes for an FPGA-based OBC

Truthfully depends on what's implemented, I will answer with what I am familiar with a SoC with Linux Cores which could run RTOS and, FPGA programmable logic.

- Determinism improves, programmable logic is more deterministic as each module can run in parallel running on each clock. There's no pipeline so each IP Core or block's timing doesn't depend on anything else
- Create abstractions of the FPGA modules so as to re-use them as much as possible down the road.
- Validate that the programmable logic can fit on the storage, it is often larger than some custom linux builds.