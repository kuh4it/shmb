#include <ntddk.h>

//example struct
typedef struct _SharedBuffer {
    ULONG Data;
} SharedBuffer, *PSharedBuffer;
 
PSharedBuffer g_SharedBuffer;

// System thread routine
VOID SystemThreadRoutine(PVOID Context) {
    UNREFERENCED_PARAMETER(Context);

    while (!KeReadStateEvent(&g_SharedBuffer->ExitEvent)) {
        //modify buff(example:)
        g_SharedBuffer->Data++;
 
        KeDelayExecutionThread(KernelMode, 0, 1000000); // 1 second
    }

    PsTerminateSystemThread(STATUS_SUCCESS);
}

// Driver unload routine
extern "C" NTSTATUS
DriverUnload(_In_ PDRIVER_OBJECT DriverObject) {
    UNREFERENCED_PARAMETER(DriverObject);
 
    KeSetEvent(&g_SharedBuffer->ExitEvent, IO_NO_INCREMENT, FALSE);

    return STATUS_SUCCESS;
}

// Driver entry point
extern "C" NTSTATUS
DriverEntry(_In_ PDRIVER_OBJECT DriverObject, _In_ PUNICODE_STRING RegistryPath) {
    UNREFERENCED_PARAMETER(RegistryPath); 
    g_SharedBuffer = (PSharedBuffer)ExAllocatePoolWithTag(NonPagedPool, sizeof(SharedBuffer), 'SHMB');
    if (!g_SharedBuffer) {
        return STATUS_INSUFFICIENT_RESOURCES;
    } 
    RtlZeroMemory(g_SharedBuffer, sizeof(SharedBuffer));
    KeInitializeEvent(&g_SharedBuffer->ExitEvent, NotificationEvent, FALSE);
 
    HANDLE ThreadHandle;
    NTSTATUS Status = PsCreateSystemThread(
        &ThreadHandle,
        THREAD_ALL_ACCESS,
        NULL,
        NULL,
        NULL,
        SystemThreadRoutine,
        NULL
    );
    Util::HideThread(&ThreadHandle);
    if (!NT_SUCCESS(Status)) {
        ExFreePoolWithTag(g_SharedBuffer, 'SHMB');
        return Status;
    }

    ZwClose(ThreadHandle);
 
    DriverObject->DriverUnload = DriverUnload;

    return STATUS_SUCCESS;
}
