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

Reports Uptime From The 1 ms Tick
    Start Board

    Execute Command           pause
    Execute Command           emulation RunFor "2"
    Write Line To Uart        uptime
    # 2 s plus boot and banner time; a wrong tick rate would be off by seconds.
    Wait For Line On Uart     ^2[01][0-9]{2} ms    treatAsRegex=true
