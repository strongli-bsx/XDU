void which_mode()
{
	//本工程中中存放烧写的程序代码，直接load进入，因此该烧写程序做了简单修改
    unsigned int i,j=0;
	unsigned char chardata;
	while(uart_r_cnt<4)   uart_r_cnt++;
	/*接收前4字节，看是不是烧写指令*/
	i=0;
	while(rrdata0[i]==burn_keys1[i]&&i<4)  i++;
	if(i==4){
		/*接收接下来的4字节，看代码长度是多少*/
		while(uart_r_cnt<8) uart_r_cnt++;
		for(i = 0; i < 4; i++){
			chardata = rrdata0[i+4];
			length |= (int)chardata << (i*8);
		}
		uart_report("Now it`s time to burn flash!\r\n");
		j=1;
	}
	else{
//		lcd_data_write3("  Wrong Reset!  ");
//		uart_report("Wrong");
//		go_main();
	}
	 /*接收后续代码字节*/
//	i=0;
//	uart_report(receiving_data);
//	while(uart_r_cnt<length+8){
//		if(!i)
//			++i;
////			lcd_data_write3("Receive code!   ");
//		}
//		progress_bar(uart_r_cnt);

//	lcd_data_write3("Receive done!   ");



	if(j==1)
	{
		erase_flash();
		burn_main();

	}
}




void erase_flash()
{
	//lcd_data_write3("Erasing!        ");
	uart_report(flash_erase);
	//FLASH_erase(FLASH_BASE, 0x3effff);
	//lcd_data_write3("Done!  ");
	Flash_ChipEraseALL();//Chip Erase
	uart_report(flash_erased);
}



void burn_main()
{
//    unsigned int i;
//	unsigned char *pfadd;

////    FLASH_read(sector_end[FLASH_SECTORS-1]-15, password, 12);
//    read password(201806250500)
//	for(i = 0; flash_ensure[i] != '\0'; i++)
//	{
//		while(UART0_T_fifo_cnt);
//		UART0_THR = flash_ensure[i];
//	}
//	char rec_password[13] = {0};
//	for(i = 0; i < 12; i++)
//	{
//		while(!UART0_R_fifo_cnt);
//		rec_password[i] = UART0_RHR;
//		if(rec_password[i] != password[i])
//		{
//			for(i = 0; errorword[i] != '\0'; i++)
//			{
//				while(UART0_T_fifo_cnt);
//				UART0_THR = errorword[i];
//			}
////			lcd_data_write3("Reset Please!!!");
//			return;
//		}
//	}






//	for(i = 0; waiting_data[i] != '\0'; i++)
//	{
//		while(UART0_T_fifo_cnt);
//		UART0_THR = waiting_data[i];
//	}
//	lcd_data_write3("Waitting data!\n");
	//pfadd =(unsigned char *)FLASHADDRESS;




//	lcd_data_write3("Code writing!  ");
	uart_report(flash_write);
//	FLASH_write_2(win16_4+8, FLASH_BASE);

	FLASH_write_lee(rrdata0+8, FLASH_BASE);//烧写时候把这条语句打开(从rrdata0 load 数据)

/*	for(;i <FlashSize; i++)
	{

		Flash_WriteByte(pfadd++,0);
	}*/

	//FLASH_erase(sector_end[FLASH_SECTORS-1]-15, 12);
	//FLASH_write(password, sector_end[FLASH_SECTORS-1]-15, 12);
	uart_report(flash_writed);
//	lcd_data_write3("DONE! Reset!    ");

	while(1);
}




void uart_report(const char * str)
{
	int i;
	for(i = 0; str[i] != '\0'; i++)//\0是字符串的结束符号
	{
		while(UART0_T_fifo_cnt);
		UART0_THR = str[i];
	}
}


