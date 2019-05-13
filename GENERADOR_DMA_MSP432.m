clear all;
close all;
clc;

ADC_TASKS = 128;    % The number of ADC samples you need 
a = 1;

L{a} = '#define MCLK_FREQUENCY      48000000';a = a+1;
L{a} = '#define SMCLK_FREQUENCY     24000000';a = a+1;
L{a} = ['#define NUMBER_OF_SAMPLES       ' num2str(ADC_TASKS)];a = a+1;
L{a} = '#define CONVERSIONS_PER_SAMPLE  8';a = a+1;
L{a} = '#define ADC_TASKS               NUMBER_OF_SAMPLES+2';a = a+1;
L{a} = ' ';a = a+1;
L{a} = 'uint32_t dataSet_01[NUMBER_OF_SAMPLES][CONVERSIONS_PER_SAMPLE];';a = a+1;
L{a} = 'uint32_t dataSet_02[NUMBER_OF_SAMPLES][CONVERSIONS_PER_SAMPLE];';a = a+1;
L{a} = 'const uint8_t forceP1B2ISR = 0x04;  // P1.2 is high';a = a+1;
L{a} = 'const uint8_t forceP1B3ISR = 0x08;  // P1.3 is high';a = a+1;
L{a} = 'bool dataSet0Ready;';a = a+1;
L{a} = ' ';a = a+1;
L{a} ='#if defined(__TI_COMPILER_VERSION__)';a = a+1;
L{a} ='#pragma DATA_ALIGN(MSP_EXP432P401RLP_DMAControlTable, 1024)';a = a+1;
L{a} ='#elif defined(__IAR_SYSTEMS_ICC__)';a = a+1;
L{a} ='#pragma data_alignment=1024';a = a+1;
L{a} ='#elif defined(__GNUC__)';a = a+1;
L{a} ='__attribute__ ((aligned (1024)))';a = a+1;
L{a} ='#elif defined(__CC_ARM)';a = a+1;
L{a} ='__align(1024)';a = a+1;
L{a} ='#endif';a = a+1;
L{a} ='static DMA_ControlTable MSP_EXP432P401RLP_DMAControlTable[16];  // 8 primary and 8 alternate';a = a+1;
L{a} ='DMA_ControlTable pingTask,pongTask;';a = a+1;
L{a} = ' ';a = a+1;
L{a} ='const DMA_ControlTable AdcDmaSeq_01[ADC_TASKS] =';a = a+1;
L{a} ='{';a = a+1;

% Here you must put the path to the CCS project
fid1 = fopen('D:\PROYECTOS2018\PROY_CODE_COM\MSP432_DMA_MULTI6\preCompile.h','wt');
for e = 1:a-1
    fprintf(fid1,'%s\n',L{e});
end

for e = 0:ADC_TASKS-1
    
    DEST = strcat('        UDMA_DST_INC_32, &dataSet_01[',num2str(e));
    DEST = strcat(DEST,'][0],');
    fprintf(fid1,'%s\n','DMA_TaskStructEntry(8, UDMA_SIZE_32,');
    fprintf(fid1,'%s\n','        UDMA_SRC_INC_32,(void*)&ADC14->MEM[0],');
    fprintf(fid1,'%s\n',DEST);
    fprintf(fid1,'%s\n','        UDMA_ARB_1, UDMA_MODE_PER_SCATTER_GATHER),');
    
end
    fprintf(fid1,'\n%s\n','DMA_TaskStructEntry(1, UDMA_SIZE_8,');
    fprintf(fid1,'%s\n','            UDMA_SRC_INC_8, (void *)&forceP1B2ISR,');
    fprintf(fid1,'%s\n','            UDMA_DST_INC_8, (void *)&P1->OUT,');
    fprintf(fid1,'%s\n','            UDMA_ARB_1, UDMA_MODE_MEM_SCATTER_GATHER),');
    fprintf(fid1,'%s\n','DMA_TaskStructEntry(4, UDMA_SIZE_32,');
    fprintf(fid1,'%s\n','            UDMA_SRC_INC_32, (void *)&pongTask,');
    fprintf(fid1,'%s\n','            UDMA_DST_INC_32, (void *)&MSP_EXP432P401RLP_DMAControlTable[7],');
    fprintf(fid1,'%s\n','            UDMA_ARB_4, UDMA_MODE_MEM_SCATTER_GATHER)');
    fprintf(fid1,'%s\n','};');
    
    fprintf(fid1,'\n%s\n','const DMA_ControlTable AdcDmaSeq_02[ADC_TASKS] =');
    fprintf(fid1,'%s\n','{');
for e = 0:ADC_TASKS-1
    
    DEST = strcat('        UDMA_DST_INC_32, &dataSet_02[',num2str(e));
    DEST = strcat(DEST,'][0],');
    fprintf(fid1,'%s\n','DMA_TaskStructEntry(8, UDMA_SIZE_32,');
    fprintf(fid1,'%s\n','        UDMA_SRC_INC_32,(void*)&ADC14->MEM[0],');
    fprintf(fid1,'%s\n',DEST);
    fprintf(fid1,'%s\n','        UDMA_ARB_1, UDMA_MODE_PER_SCATTER_GATHER),');
    
end
    fprintf(fid1,'\n%s\n','DMA_TaskStructEntry(1, UDMA_SIZE_8,');
    fprintf(fid1,'%s\n','            UDMA_SRC_INC_8, (void *)&forceP1B2ISR,');
    fprintf(fid1,'%s\n','            UDMA_DST_INC_8, (void *)&P1->OUT,');
    fprintf(fid1,'%s\n','            UDMA_ARB_1, UDMA_MODE_MEM_SCATTER_GATHER),');
    fprintf(fid1,'%s\n','DMA_TaskStructEntry(4, UDMA_SIZE_32,');
    fprintf(fid1,'%s\n','            UDMA_SRC_INC_32, (void *)&pingTask,');
    fprintf(fid1,'%s\n','            UDMA_DST_INC_32, (void *)&MSP_EXP432P401RLP_DMAControlTable[7],');
    fprintf(fid1,'%s\n','            UDMA_ARB_4, UDMA_MODE_MEM_SCATTER_GATHER)');
    fprintf(fid1,'%s\n','};');    
    
fclose(fid1);


