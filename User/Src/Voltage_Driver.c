#include "Voltage_Driver.h"


__IO uint32_t ADC_ConvertedValue;
DMA_HandleTypeDef DMA_Init_Handle;
ADC_HandleTypeDef ADC_Handle;
ADC_ChannelConfTypeDef ADC_Config;

static void Voltage_ADC_GPIO_Config(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    VOLTAGE_ADC_CLK_ENABLE(); 
    // ʹ�� GPIO ʱ��
    VOLTAGE_ADC_GPIO_CLK_ENABLE();
          
    // ���� IO
    GPIO_InitStructure.Pin = VOLTAGE_ADC_GPIO_PIN;
    GPIO_InitStructure.Mode = GPIO_MODE_ANALOG;	    
    GPIO_InitStructure.Pull = GPIO_NOPULL ; //������������
    HAL_GPIO_Init(VOLTAGE_ADC_GPIO_PORT, &GPIO_InitStructure);		
}

static void Voltage_ADC_Mode_Config(void)
{
    RCC_PeriphCLKInitTypeDef ADC_CLKInit;
    // ����ADCʱ��
    ADC_CLKInit.PeriphClockSelection = RCC_PERIPHCLK_ADC;			//ADC����ʱ��
    ADC_CLKInit.AdcClockSelection = RCC_ADCPCLK2_DIV6;			  //��Ƶ����6ʱ��Ϊ72M/6=12MHz
    HAL_RCCEx_PeriphCLKConfig(&ADC_CLKInit);					      //����ADCʱ��
   
    ADC_Handle.Instance = VOLTAGE_ADC;
    ADC_Handle.Init.DataAlign = ADC_DATAALIGN_RIGHT;             //�Ҷ���
    ADC_Handle.Init.ScanConvMode = DISABLE;                      //��ɨ��ģʽ
    ADC_Handle.Init.ContinuousConvMode = ENABLE;                 //����ת��
    ADC_Handle.Init.NbrOfConversion = 1;                         //1��ת���ڹ��������� Ҳ����ֻת����������1 
    ADC_Handle.Init.DiscontinuousConvMode = DISABLE;             //��ֹ����������ģʽ
    ADC_Handle.Init.NbrOfDiscConversion = 0;                     //����������ͨ����Ϊ0
    ADC_Handle.Init.ExternalTrigConv = ADC_SOFTWARE_START;       //��������
    HAL_ADC_Init(&ADC_Handle);                                 //��ʼ�� 
 
 //---------------------------------------------------------------------------
    ADC_Config.Channel      = VOLTAGE_ADC_CHANNEL;
    ADC_Config.Rank         = 1;
    // ����ʱ����	
    ADC_Config.SamplingTime = ADC_SAMPLETIME_55CYCLES_5 ;
    // ���� ADC ͨ��ת��˳��Ϊ1����һ��ת��������ʱ��Ϊ3��ʱ������
    HAL_ADC_ConfigChannel(&ADC_Handle, &ADC_Config);

    HAL_ADC_Start_IT(&ADC_Handle);
}

// �����ж����ȼ�
static void Voltage_ADC_NVIC_Config(void)
{
  HAL_NVIC_SetPriority(Voltage_ADC_IRQ, 0, 0);
  HAL_NVIC_EnableIRQ(Voltage_ADC_IRQ);
}

void Voltage_Init(void)
{
	Voltage_ADC_GPIO_Config();
	Voltage_ADC_Mode_Config();
  Voltage_ADC_NVIC_Config();
}

/**
  * @brief  ת������жϻص�������������ģʽ��
  * @param  AdcHandle : ADC���
  * @retval ��
  */
void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* AdcHandle)
{
  /* ��ȡ��� */
  ADC_ConvertedValue = HAL_ADC_GetValue(AdcHandle);
}


/***************************************
**�������ܣ���ȡ�ɼ���ѹֵ
**��	ʽ��V(sample) = V(REF) * Value/(0x0FFF + 1)
****************************************/
float Get_VoltageValue(void)
{
	float Value;
 //(float) ADC_ConvertedValue/4096*(float)3.3; // ��ȡת����ADֵ
	Value = (float)ADC_ConvertedValue * ((float)3.3 / 4096);

	return Value;
}

