#include "sys.h"

void NVIC_Configuration(void)
{
    //配置NVIC中断分组
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);	

}
