/*
 * SLIP_trs.c
 *
 * Created on: 2015-8-6
 * Author: Administrator
 */
#include "../register.h"
#include "../hardware_init.h"
#include  "../qtab.h"
#include <stdio.h>
#include<string.h>

extern unsigned char send_wave_ID;
extern unsigned char send_interweave;
extern unsigned char send_test_bit;
extern unsigned char send_ARQ_bit;
void tran_deal_main();
extern volatile unsigned short recv_buf_ip;
extern volatile unsigned short recv_buf_op;
extern unsigned short send_buf_ip;
extern unsigned short send_buf_op;
extern void go_main();
int mm1=1,mm2=0,start = 0;
void set_channel_jj(unsigned char data);
void set_channel(unsigned char data);
void end_data();
void isdatareport();
unsigned char channel_rate = 0xff;
//unsigned short channel_rate1 = 0xff;//channel_rate1::波形号1
//unsigned short channel_rate2 = 0xff;//channel_rate2::波形号2
int data_report = 0;
int chann_flag=0;//信道参数接收完毕标志
int chann_flag1=0;//信道参数接收完毕标志
int chann_cnt=0;//用于监视信道参数接收完毕
int chann_cnt1=0;//用于监视信道参数接收完毕
//short IOsigFlag=0;//用于表示发送数据信息是来自上位机还是内部产生的信号::0:内部信号；1：上位机信号
int IOend=1;
int WaveNumEnd=0;

unsigned char testm[3]={0};
int testm_cnt=0;

unsigned int chann2_cnt=0;
unsigned int chann3_cnt=0;
unsigned int chann4_cnt=0;

unsigned int c_cnt1=0,c_cnt2=0,c_cnt3=0;
volatile unsigned int c_cnt_flag=1;//接收完上位机控制或信道参数字节的标志
int exit_flag=0;
int ij=0;
unsigned char LED_buff = 0;
void zero(){

}


void SLIP_trs()
{
	int temp=0,r_fifo=0;//temp为剩余缓冲区
	int i=0;
	if (++xxms>500){
			xxms = 0;
			LED_buff &= 7;
			LED_buff ^= 7;
			LED = LED_buff;
		}

//从空中接收数据，经过处理后，通过端口发给上位机      rr_mod——DSP接收到信号标志，也即 DSP向串口上报数据
	if(rr_mod==1)
	{
	    if (((z_step - t_step) & 0xfff) != 0) //
	    {
	    	while(UART0_T_fifo_cnt&&(wtdata[t_step] != -1));//-1::结束符？
	    	UART0_THR = wtdata[t_step]&0xff;
	    	t_step = 0xfff & (t_step + 1);

	    }
	    if (wtdata[t_step] == -1)
	    {
	    	t_step = z_step;
	     }

	}

//端口从上位机接收数据
	temp=UART0_Flow_control_register&0xff;
/*************************执行1**begin***********************/

	if(msr!=temp)
    {//检测到上位机的rts改变
    	if(!(temp&0x10))
    	{//检测到上位机的rts打上勾
    		if(control3 == -1)
    		{//此时没有正在接收的信号                       (control3==-1：此时没有空中接收信号，没来空中信号持续发送空白符
    			UART0_Flow_control_register=0x00;//将上位机的cts打上勾
    			msr=UART0_Flow_control_register&0xff;//msr=0;所以这一段只执行一次
    			start = 1;//表明可以设置速率，只设置一次
    			uart_r_cnt=0;
    			//rts_flag=1;
    			//control3 = -1;
    			//memset(SD_DTE,0,sizeof(SD_DTE));
    		//	memset(SD_DTE,0,SI_DTEP);//SI_DTEP：SD_DTE数组的存指针    memset：这个函数干嘛的？0：将数组SD_DTE清零；SI_DTEP：数组长度
    		//	SI_DTEP=0;				 //memset函数：给分配的内存进行初始化
    		//	SO_DTEP=0;//SD_DTE数组的取指针
    		}
    	}

    	else
    	{//上位机的rts改变了，但未检测到上位机的rts勾？？？  //上位机去掉rts勾
    		if(control3 == -1)
    		{//此时没有正在接收的信号
    			if(start)
    			{
    				UART0_Flow_control_register=0x01;//将上位机的cts的勾去掉
    				msr=UART0_Flow_control_register&0xff;
    		    	wtdata[uart_r_cnt++]=-1;//将从串口接收的数据存放到
    		    	uart_r_cnt=uart_r_cnt &0xfff;
    		    	wtdata[uart_r_cnt++]=-1;//将从串口接收的数据存放到
    		    	uart_r_cnt=uart_r_cnt &0xfff;
    		    	wtdata[uart_r_cnt++]=-1;//将从串口接收的数据存放到
    		    	uart_r_cnt=uart_r_cnt &0xfff;
    			}
    			if(!bps)
    			{    				//bps作用？:用于显示控制
    				while(UART0_R_fifo_cnt)//清空UART缓存的数据
    					temp=UART0_RHR;
    			//	end_data();			//填结束符号0x065cd44f4b65a5b2？？？？？？
    				bps=1;
    				return;
    			}
    		}
    	 }
    }

/*************************执行1**end***********************/

/**********************接收上位机首字节**********************************/
    if(start==1&&(IOend==1))
    {
    	while(UART0_R_fifo_cnt&&(IOend==1))
    	{
        SLIP_NUM = UART0_RHR&0xFF;
        PLLDelay(50);
        testm[testm_cnt++]=SLIP_NUM;
      //  while(UART0_T_fifo_cnt);
        UART0_THR = SLIP_NUM;
        PLLDelay(50);


        if(testm_cnt>=4)
        {
        	 testm_cnt=0;
        	 if(testm[0]==0xff&&testm[1]==0xee&&(testm[2]==0x0||testm[2]==0x1))
        	 {
        	     test_mode=testm[2];
        	     channel_open=testm[3] &0x0f;
        	     AGC_num=(testm[3]>>4)&15;
        	     IOend=2;//首字节接收完成

        	     if(!test_mode)
        	         lcd_data_write(welcome[0]);
        	     else
        	         lcd_data_write(welcome1[0]);

        	 }
        	 /*---------------------判断是否是烧写程序-------------------------*/
        	 else if((testm[0]==0x4B)&&(testm[1]==0x65)&&(testm[2]==0xA5)&&(testm[3]==0xB2))
        	 {
        		 while(uart_r_cnt1<4)
        			 rrdata0[uart_r_cnt1++]=testm[uart_r_cnt1];//接收烧写程序
        		 IOend=3;//首字节接收完成
       	 	}

        	 else
        	 {
        		 for(ij=0;ij<4;ij++)
        		 {
        			 wtdata[uart_r_cnt++]=(short) testm[testm_cnt++];
        			 if(testm_cnt>=4) IOend=2;
        			 if(ij>=4)ij=0;

        		 }

        	 }

        }

//        if((SLIP_NUM==0x0)||(SLIP_NUM==0x1))//发送业务标志
//        {
//        	test_mode=SLIP_NUM;
//        	IOend=2;//首字节接收完成
//
//        }


       // if(uart_r_cnt>=2)IOend=0;//uart_r_cnt=2
       // IOend=2;//首字节接收完成
        //上报
     //   while(UART0_T_fifo_cnt);
      //  UART0_THR = SLIP_NUM;
    	}
    }



/************************执行4**begin*******************************/
    if(start==1&&(IOend==2))//接受上位机来的数据信息
    {
    	while(UART0_R_fifo_cnt)
    	{
    		SLIP_NUM = UART0_RHR&0xFF;
    		wtdata[uart_r_cnt++]=(short) SLIP_NUM;//将从串口接收的数据存放到

    		if(uart_r_cnt>=0x1000) uart_r_cnt=0;
    		//uart回掷---test begin
    	//	while(UART0_T_fifo_cnt);
    	//	UART0_THR = SLIP_NUM;//同时将从串口接收到的数据给到发送端UART0_THR返回到串口
    		//uart回掷---test end
    			rts_flag=1;

//    		timer0_cnt=0;

    	}

    }



    if(start==1&&(IOend==3))//接受上位机来的烧写程序数据信息
    {

    	while (uart_r_cnt1 < 8 ) {
			if (UART0_R_fifo_cnt) {
				SLIP_NUM = UART0_RHR & 0xFF;
				rrdata0[uart_r_cnt1++] = SLIP_NUM;
			}
		}
    	unsigned char chardata;
    	for(i = 0; i < 4; i++){
    				chardata = rrdata0[i+4];
    				length |= (int)chardata << (i*8);
    			}

    	PLLDelay(2000);

		while (uart_r_cnt1 < length + 8 || uart_r_cnt1 > 0x30000) {

			while (UART0_R_fifo_cnt ) {
				SLIP_NUM = UART0_RHR & 0xFF;
				rrdata0[uart_r_cnt1++] = SLIP_NUM;    	//将从串口接收的数据存放到rrdata0
//    		UART0_THR = SLIP_NUM;//同时将从串口接收到的数据给到发送端UART0_THR返回到串口
//    		if(uart_r_cnt1>=0x200000) uart_r_cnt1=0;
						//uart回掷---test begin
//    		while(UART0_T_fifo_cnt);
						//uart回掷---test end
				rts_flag = 2; //烧写串口程序
//    		timer0_cnt=0;
			}
		}
	}

    if(rts_flag==2){
    	PLLDelay(2000000);
    	which_mode();
    }



/*************************执行4**end***********************/

//经过试验发现，上位机来的数据不能一下全部传给下位机，可能存在多次传输，要确保数据接收完成
//每次重新从上位机接收数据，将上位机rts摘掉，cts跟着摘掉
    if((msr=UART0_Flow_control_register&0x10))//上位机的rts摘掉了
    {
        UART0_Flow_control_register=0x11;//将上位机的cts的勾去掉
        msr=UART0_Flow_control_register&0xff;

    }


//    if(start&&UART0_R_fifo_cnt&&(!msr)&&(IOend))
//    {
//
//    	//接受前两个字节数据
//    	while(UART0_R_fifo_cnt&&IOend)
//    	{
//    		SLIP_NUM = UART0_RHR&0xFF;
//    		PLLDelay(50);
//    		rrdata0[uart_r_cnt++]=SLIP_NUM;
//    		if(uart_r_cnt>=2)IOend=0;//uart_r_cnt=2
//    		//上报
//    		while(UART0_T_fifo_cnt);
//    		UART0_THR = SLIP_NUM;
//    	}
//
//    }
//
//
//    //接受到前两个字节数据，判断是否需要设置信道参数
//    if(start&&UART0_R_fifo_cnt&&(!msr)&&(!IOend)&&(!chann_flag))//IOend=0：：表示前两字节接收完成
//    {

//    	if(rrdata0[0]==0xff&&(rrdata0[1]>0&&rrdata0[1]<=4))
//    	{
//    		chann_flag1=1;
//    		//接受信道参数
//    		if(rrdata0[1]==1)
//    		{
//    			while(UART0_R_fifo_cnt&&(!chann_flag))
//    			{
//    			    SLIP_NUM = UART0_RHR&0xFF;
//    			    PLLDelay(50);
//    			    rrdata0[uart_r_cnt++]=SLIP_NUM;//将从串口接收的数据存放到SD_DTE[0x4000]????
//    			    //uart回掷---test begin
//    			    chann_cnt++;
//    			    while(UART0_T_fifo_cnt);
//    			    UART0_THR = SLIP_NUM;//同时将从串口接收到的数据给到发送端UART0_THR返回到串口？？？？实际中去掉这一句不上报？？？
//    			    //uart回掷---test end
//    			    if(chann_cnt>rrdata0[2]) chann_flag=1;
//
//    			 }
//    		}
//    		if(rrdata0[1]==2)
//    		{
//    		    while(UART0_R_fifo_cnt&&(!chann_flag))
//    		    {
//    		    	SLIP_NUM = UART0_RHR&0xFF;
//    		    	PLLDelay(50);
//    		    	rrdata0[uart_r_cnt++]=SLIP_NUM;//将从串口接收的数据存放到SD_DTE[0x4000]????
//    		    	//uart回掷---test begin
//    		    	chann_cnt++;
//    		    	while(UART0_T_fifo_cnt);
//    		    	UART0_THR = SLIP_NUM;//同时将从串口接收到的数据给到发送端UART0_THR返回到串口？？？？实际中去掉这一句不上报？？？
//    		    	//uart回掷---test end
//    		    	if(chann_cnt>rrdata0[2])//chann_cnt>rrdata0[2]::表示第一组信道参数接收完成
//    		    	{
//    		    		chann_cnt=0;
//    		    		chann2_cnt=uart_r_cnt;//chann2_cnt第二组信道参数开始下标：：rrdata0[chann2_cnt]:表示第二组信道参数长度
//    		    		while(UART0_R_fifo_cnt&&(!chann_flag))
//    		    		{
//    		    			SLIP_NUM = UART0_RHR&0xFF;
//    		    			PLLDelay(50);
//    		    			rrdata0[uart_r_cnt++]=SLIP_NUM;//将从串口接收的数据存放到SD_DTE[0x4000]????
//    		    			//uart回掷---test begin
//    		    			chann_cnt++;
//    		    			while(UART0_T_fifo_cnt);
//    		    			UART0_THR = SLIP_NUM;//同时将从串口接收到的数据给到发送端UART0_THR返回到串口？？？？实际中去掉这一句不上报？？？
//    		    			//uart回掷---test end
//    		    			if(chann_cnt>rrdata0[chann2_cnt])chann_flag=1;
//
//    		    		}
//    		    	}
//
//    		    }
//    		}
//    		if(rrdata0[1]==3)
//    		{
//    			 while(UART0_R_fifo_cnt&&(!chann_flag))
//    			 {
//    			    SLIP_NUM = UART0_RHR&0xFF;
//    			    PLLDelay(50);
//    			    rrdata0[uart_r_cnt++]=SLIP_NUM;//将从串口接收的数据存放到SD_DTE[0x4000]????
//    			    //uart回掷---test begin
//    			    chann_cnt++;
//    			    while(UART0_T_fifo_cnt);
//    			    UART0_THR = SLIP_NUM;//同时将从串口接收到的数据给到发送端UART0_THR返回到串口？？？？实际中去掉这一句不上报？？？
//    			    //uart回掷---test end
//    			    if(chann_cnt>rrdata0[2])//chann_cnt>rrdata0[2]::表示第一组信道参数接收完成
//    			    {
//    			    	chann_cnt=0;
//    			    	chann2_cnt=uart_r_cnt;//chann2_cnt第二组信道参数开始下标：：rrdata0[chann2_cnt]:表示第二组信道参数长度
//    			    	while(UART0_R_fifo_cnt&&(!chann_flag))
//    			    	{
//    			    		SLIP_NUM = UART0_RHR&0xFF;
//    			    		PLLDelay(50);
//    			    		rrdata0[uart_r_cnt++]=SLIP_NUM;//将从串口接收的数据存放到SD_DTE[0x4000]????
//    			    		//uart回掷---test begin
//    			    		chann_cnt++;
//    			    		while(UART0_T_fifo_cnt);
//    			    		UART0_THR = SLIP_NUM;//同时将从串口接收到的数据给到发送端UART0_THR返回到串口？？？？实际中去掉这一句不上报？？？
//    			    		//uart回掷---test end
//    			    		if(chann_cnt>rrdata0[chann2_cnt])//chann_cnt>rrdata0[chann2_cnt]::表示第2组信道参数接收完成
//    			    		{
//    			    			chann_cnt=0;
//    			    			chann3_cnt=uart_r_cnt;//chann3_cnt第二组信道参数开始下标：：rrdata0[chann3_cnt]:表示第3组信道参数长度
//    			    			while(UART0_R_fifo_cnt&&(!chann_flag))
//    			    			{
//    			    			    SLIP_NUM = UART0_RHR&0xFF;
//    			    			    PLLDelay(50);
//    			    			    rrdata0[uart_r_cnt++]=SLIP_NUM;//将从串口接收的数据存放到SD_DTE[0x4000]????
//    			    			   	//uart回掷---test begin
//    			    			    chann_cnt++;
//    			    			    while(UART0_T_fifo_cnt);
//    			    			    UART0_THR = SLIP_NUM;//同时将从串口接收到的数据给到发送端UART0_THR返回到串口？？？？实际中去掉这一句不上报？？？
//    			    			    //uart回掷---test end
//    			    			    if(chann_cnt>rrdata0[chann3_cnt]) chann_flag=1;
//    			    		    }
//
//    			    	   }
//    			      }
//
//    			  }
//    		   }
//
//    		}
//    		if(rrdata0[1]==4)
//    		{
//    			 while(UART0_R_fifo_cnt&&(!chann_flag))
//    			 {
//    			    SLIP_NUM = UART0_RHR&0xFF;
//    			    PLLDelay(50);
//    			    rrdata0[uart_r_cnt++]=SLIP_NUM;//将从串口接收的数据存放到SD_DTE[0x4000]????
//    			    //uart回掷---test begin
//    			    chann_cnt++;
//    			    while(UART0_T_fifo_cnt);
//    			    UART0_THR = SLIP_NUM;//同时将从串口接收到的数据给到发送端UART0_THR返回到串口？？？？实际中去掉这一句不上报？？？
//    			    //uart回掷---test end
//    			    if(chann_cnt>rrdata0[2])//chann_cnt>rrdata0[2]::表示第一组信道参数接收完成
//    			    {
//    			    	chann_cnt=0;
//    			    	chann2_cnt=uart_r_cnt;//chann2_cnt第二组信道参数开始下标：：rrdata0[chann2_cnt]:表示第二组信道参数长度
//    			    	while(UART0_R_fifo_cnt&&(!chann_flag))
//    			    	{
//    			    		SLIP_NUM = UART0_RHR&0xFF;
//    			    		PLLDelay(50);
//    			    		rrdata0[uart_r_cnt++]=SLIP_NUM;//将从串口接收的数据存放到SD_DTE[0x4000]????
//    			    		//uart回掷---test begin
//    			    		chann_cnt++;
//    			    		while(UART0_T_fifo_cnt);
//    			    		UART0_THR = SLIP_NUM;//同时将从串口接收到的数据给到发送端UART0_THR返回到串口？？？？实际中去掉这一句不上报？？？
//    			    		//uart回掷---test end
//    			    		if(chann_cnt>rrdata0[chann2_cnt])//chann_cnt>rrdata0[chann2_cnt]::表示第2组信道参数接收完成
//    			    		{
//    			    			 chann_cnt=0;
//    			    			 chann3_cnt=uart_r_cnt;//chann3_cnt第二组信道参数开始下标：：rrdata0[chann3_cnt]:表示第3组信道参数长度
//    			    			 while(UART0_R_fifo_cnt&&(!chann_flag))
//    			    			 {
//    			    			    SLIP_NUM = UART0_RHR&0xFF;
//    			    			    PLLDelay(50);
//    			    			    rrdata0[uart_r_cnt++]=SLIP_NUM;//将从串口接收的数据存放到SD_DTE[0x4000]????
//    			    			    //uart回掷---test begin
//    			    			    chann_cnt++;
//    			    			    while(UART0_T_fifo_cnt);
//    			    			    UART0_THR = SLIP_NUM;//同时将从串口接收到的数据给到发送端UART0_THR返回到串口？？？？实际中去掉这一句不上报？？？
//    			    			    //uart回掷---test end
//    			    			    if(chann_cnt>rrdata0[chann3_cnt]) //chann_cnt>rrdata0[chann3_cnt]::表示第3组信道参数接收完成
//    			    			    {
//    			    			    	 chann_cnt=0;
//    			    			    	 chann4_cnt=uart_r_cnt;//chann4_cnt第二组信道参数开始下标：：rrdata0[chann4_cnt]:表示第4组信道参数长度
//    			    			    	 while(UART0_R_fifo_cnt&&(!chann_flag))
//    			    			    	 {
//    			    			    	     SLIP_NUM = UART0_RHR&0xFF;
//    			    			    	     PLLDelay(50);
//    			    			    	     rrdata0[uart_r_cnt++]=SLIP_NUM;//将从串口接收的数据存放到SD_DTE[0x4000]????
//    			    			    	     //uart回掷---test begin
//    			    			    	     chann_cnt++;
//    			    			    	     while(UART0_T_fifo_cnt);
//    			    			    	     UART0_THR = SLIP_NUM;//同时将从串口接收到的数据给到发送端UART0_THR返回到串口？？？？实际中去掉这一句不上报？？？
//    			    			    	     //uart回掷---test end
//    			    			    	     if(chann_cnt>rrdata0[chann4_cnt]) chann_flag=1;
//
//    			    			    	 }
//
//
//    			    			 }
//
//    			    		}
//    			    	}
//
//    			   }
//    		    }
//
//    		}
//
//    		}
//    	}
//    	else//前两个字节数据不是设置信道参数的命令
//    	{
//
//    		//int i=0;
//    		IOsigFlag=rrdata0[0];//
//    		tnum1=rrdata0[1];//channel_rate1::波形号1
//    		rts_flag=1;
//
//    		timer0_cnt=0;
//    		chann_flag=1;
//    		chann_cnt1=1;
//    		ff=1;//
//
//
//    	}
//
//    }
//
////	if(start&&UART0_R_fifo_cnt&&(!msr)&&(ff==1))//ff==1：：表示前两字节不是信道参数，开始接收第二个波形号
////	{
////		while(UART0_R_fifo_cnt&&ff)
////		{
////			SLIP_NUM = UART0_RHR&0xFF;
////			PLLDelay(50);
////			rrdata0[uart_r_cnt++]=SLIP_NUM;
////			channel_rate2=SLIP_NUM;//channel_rate2::波形号2
////			ff=0;//接收一次波型号2，跳出
////			while(UART0_T_fifo_cnt);
////			UART0_THR = SLIP_NUM;
////		}
////		 c_cnt_flag=0;
////	}
//
//
//
//    //该if判断主要判断是为设置信道参数后，则接受IOsigFlag,及两个波形号
//    if(start&&UART0_R_fifo_cnt&&(!msr)&&(chann_flag1==1)&&(!WaveNumEnd))//chann_flag==1::表示信道参数接受完成
//    {
//    	int j=0;
//    	while(UART0_R_fifo_cnt&&(!WaveNumEnd))//该段只进入一次，用于接收IOsigFlag表示IOsigFlag
//    	{
//    		SLIP_NUM = UART0_RHR&0xFF;
//    		PLLDelay(50);
//    		rrdata0[uart_r_cnt]=SLIP_NUM;//从信道参数结束开始接受IOsigFlag和两个波形号
//    		uart_r_cnt++;
//    		j++;
//    		while(UART0_T_fifo_cnt);
//    		UART0_THR = SLIP_NUM;//同时将从串口接收到的数据给到发送端UART0_THR返回到串口？？？？实际中去掉这一句不上报？？？
//    		//uart回掷---test end
//    		if(j>2)
//    			WaveNumEnd=1;
//    	}
//    	c_cnt_flag=0;
//
//    	c_cnt1=uart_r_cnt-3;
//    	c_cnt2=uart_r_cnt-2;
//    	c_cnt3=uart_r_cnt-1;
//    	IOsigFlag=rrdata0[c_cnt1];
//    	tnum1=rrdata0[c_cnt2];
//  //  	channel_rate2=rrdata0[c_cnt3];
//     }
//
////    //经过试验发现，上位机来的数据不能一下全部传给下位机，可能存在多次传输，要确保数据接收完成
//    if((msr=UART0_Flow_control_register&0x10))//上位机的rts摘掉了
//    {
//    	UART0_Flow_control_register=0x11;//将上位机的cts的勾去掉
//    	msr=UART0_Flow_control_register&0xff;
//
//    }
//
//
///************************执行3*******************************/
//    if(IOsigFlag==1)//接受上位机来的数据信息
//    {//接收业务数据，不包括第一个控制字？？？？？
//		while(UART0_R_fifo_cnt&&(!msr)&&(chann_cnt1))
//		{
//			SLIP_NUM = UART0_RHR&0xFF;
//			SD_DTE[SI_DTEP++]=SLIP_NUM;//将从串口接收的数据存放到SD_DTE[0x4000]????
//			//uart回掷---test begin
//			while(UART0_T_fifo_cnt);
//			UART0_THR = SLIP_NUM;//同时将从串口接收到的数据给到发送端UART0_THR返回到串口？？？？实际中去掉这一句不上报？？？
//			//uart回掷---test end
//			SI_DTEP&=0XFFF;//最大0x03fff
//			rts_flag=1;
//			timer0_cnt=0;
//
//		}
//
//	}
///**************************************************************/

//    /************************执行4*******************************/
//        if(start==1)//接受上位机来的数据信息
//        {
//    		while(UART0_R_fifo_cnt)
//    		{
//    			SLIP_NUM = UART0_RHR&0xFF;
//    			 rrdata0[uart_r_cnt++]=SLIP_NUM;//将从串口接收的数据存放到rrdata0
//
//    			 if(uart_r_cnt>=0x200000) uart_r_cnt=0;
//    			 //uart回掷---test begin
//    			while(UART0_T_fifo_cnt);
//    			UART0_THR = SLIP_NUM;//同时将从串口接收到的数据给到发送端UART0_THR返回到串口？？？？实际中去掉这一句不上报？？？
//    			//uart回掷---test end
//    			rts_flag=1;
//    			//timer0_cnt=0;
//
//    		}
//
//    	}
//
//	else if(send_flag==2)
//	{//接收待烧写字节 ？？？？
//		while(UART0_R_fifo_cnt&&(!msr))
//		{
//			SLIP_NUM = UART0_RHR&0xFF;
//			rrdata0[uart_r_cnt++]=SLIP_NUM;
//			uart_r_cnt&=0x1FFFFF;
//		}
//	}
//	else if(send_flag==3)
//	{//接收待更改参数
//		while(UART0_R_fifo_cnt&&(!msr))
//		{
//
//		}
//	}
//	if(lcd_switch) //初始值为0，这里一直未执行
//		EMIFA_CFG_CE3CFG =0x0FFFFFFC;//0x00240120;  /* 8-bit async, 10 cycle read/write strobe */

}

