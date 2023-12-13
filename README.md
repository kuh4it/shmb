# shmb

communicate by mapping memory from within ring3 context that gets shared and communicated with in ring0

eac/be thread handling:

- don't disable APC queuing which will flag you. instead: set state to pending every frame and never send back the request.

- remove entry from the PspCidTable and don't clear it fully.

- spoof thread start address, creation time etc.. read ETHREAD kernel structure

- reroute driver object
