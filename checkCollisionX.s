.global checkCollisionX

@ Input parameters (loaded into registers)
@ R0 = r1.x
@ R1 = r1.w
@ R2 = r2.x
@ R3 = r2.w

checkCollisionX:
    @ Save registers that will be modified
    PUSH {R4-R7, LR}

    @ Check if r1 is to the left of r2
    @ r1.x + r1.w < r2.x
    ADD R8, R0, R1
    CMP R8, R2
    BLT noCollision

    @ Check if r1 is to the right of r2
    @ r1.x > r2.x + r2.w
    ADD R8, R2, R2
    CMP R0, R8
    BGT noCollision

    @ There is a horizontal collision
    MOV R0, #1
    B done

noCollision:
    @ There is no horizontal collision
    MOV R0, #0

done:
    @ Restore registers and return
    POP {R4-R7, PC}
    BX LR

