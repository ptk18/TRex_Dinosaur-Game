.data
.balign 4
message: .asciz "Your score is : %d\n"   @ Score output message in terminal

.text
.global main
.extern gameDisplay                      @ C function for graphic
.extern getTime                          @ C function for getting current time in seconds

main:
    BL getTime
    MOV R5, R0                           @ store current time in R1 before gameDisplay

    BL gameDisplay

    BL getTime
    MOV R6, R0                           @ store current time in R2 after gameDisplay (game over)
    SUB R2, R6, R5                       @ Subtract R6 from R5 to get time difference (game duration)
    MOV R4, #100
    MUL R1, R2, R4                       @ Multiply time difference by 100 to get final score

    LDR R0, =message
    BL printf                            @ Print score to terminal

end:
    MOV R0, #0
    MOV R7, #1
    SVC #0
