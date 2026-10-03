*** Variables ***
${SCRIPT}       ${CURDIR}/stm32f4.resc
${UART}         sysbus.usart2
${LED}          sysbus.gpioPortD.UserLED

*** Keywords ***
Start Board
    Execute Script            ${SCRIPT}
    Create Terminal Tester    ${UART}    defaultPauseEmulation=true
    Start Emulation
    Wait For Line On Uart     stm32-uart-driver, type help

*** Test Cases ***
Switches The Green LED
    Start Board
    Create LED Tester         ${LED}    defaultTimeout=1

    Write Line To Uart        led on
    Assert LED State          true
    Write Line To Uart        led off
    Assert LED State          false

Sends Telemetry Every Second
    Start Board
    ${telemetry}=    Create Terminal Tester    sysbus.usart3    binaryMode=true

    # First status frame: ID 0x01, uptime 1000 ms, LED off, CRC-16; COBS-encoded, 0x00-terminated.
    Wait For Bytes On Uart    04 01 e8 03 01 01 03 4d e9 00    testerId=${telemetry}    pauseEmulation=true
    # Sent about 1 s into the run by the 1 ms tick; a wrong tick rate would be far off.
    ${time}=    Execute Command    machine ElapsedVirtualTime
    Should Match Regexp       ${time}    Elapsed Virtual Time: 00:00:(00\\.9|01\\.0)
