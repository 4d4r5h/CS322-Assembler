; Gives the absolute sum of an array
; Answer in M[4092]
; Made by Adarsh Kumar

ldc 0x1000 ; a=4096
a2sp ; sp=4096
call main ; a=ra
HALT

main:
    adj -3 ; sp=4093
    stl 0  ; M[4093]=ra
    ldc n
    stl 1 ; M[4094]=n
    ldc -1
    stl 2  ; M[4095]=i=-1
    adj -1 ; sp=4092
    ldc 0
    stl 0 ; M[4092]=answer=0
    adj 1  ; sp=4093

loop:
    ldc array ; a=address of array
    ldl 2 ; a=M[4095]=i
    add  ; a=array+i
    ldnl 0 ; a=M[array+i]
    stl -2 ; M[4091]=a
    ldl -2 ; a=M[4091]
    brlz neg
jumpn:
    ldl -1 ; a=answer
    ldl -2 ; a=M[array+i], b=answer
    add 
    stl -1
    br pos
neg:
    ldl -1 ; a=answer
    ldl -2 ; a=M[array+i], b=answer
    sub ; a=b-a
    stl -1 ; answer=a
pos:
    ldl 2  ; b=a, a=i 
    adc 1 ; a=i+1
    stl 2 ; M[4095]=i+1
    ldl 2 ; a=i+1
    ldl 1 ; b=i+1, a=n
    sub ; a=i+1-n
    brz exit
    call loop
exit:
    ldl 0
    return



n: SET 10
array: data 452
data -193
data -1903
data 1293
data -1
data 0
data 221
data -999
data -1293
data 29