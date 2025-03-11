.section .text

.global nnMain

nnMain:
    bl nninitRegion
    bl nninitLocale
    bl nninitSystem
    bl nninitStartUp
    bl __cpp_initialize__aeabi_
    bl nninitCallStaticInitializers
    bl nninitSetup
    bl nnMain_
    bx lr
