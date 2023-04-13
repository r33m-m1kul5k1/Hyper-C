# Extended Page Tables

Virtualization of the guest physical memory.
The ept tables format is the same as the guest paging tables.

* page-walk length is four.

* the ept tables format are described at 28.3.2

## EPT-Induced VM Exits

* EPT misconfiguration (conditions are described at 28.3.3.1)
* EPT violation
* page-modification log-full event

mode-based execute control for EPT