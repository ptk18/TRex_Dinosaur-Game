.global checkCollisionY

@ Input parameters (loaded into registers)
@ R0 = r1.y
@ R1 = r1.h
@ R2 = r2.y
@ R3 = r2.h

checkCollisionY:
    @ Save registers that will be modified
    PUSH {R4-R7, LR}

    @ Check if r1 is above r2
    @ r1.y + r1.h < r2.y
    ADD R8, R0, R1
    CMP R8, R2
    BLT noCollision

    @ Check if r1 is below r2
    @ r1.y > r2.y + r2.h
    ADD R8, R2, R3
    CMP R0, R8
    BGT noCollision

    @ There is a vertical collision
    MOV R0, #1
    B done

noCollision:
    @ There is no vertical collision
    MOV R0, #0

done:
    @ Restore registers and return
    POP {R4-R7, PC}
    BX LR

