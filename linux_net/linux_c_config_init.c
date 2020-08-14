struct info_t{
  char name[20];
  char data[60];
};
int GetFile_Info(char *name,char *result,int size)
{
  char linebuffer[100] = {0};
  char buffer1[100] = {0};
  char buffer2[100] = {0};
  uint8_t flag = 0;
  int buffer_size=0;
  FILE *fp = fopen(STR_NETWORKCFG_FILE, "r");
  if(fp == NULL)
  {
      //printf("open error");
      return 1;
  }
  while(1)
  {
      char *ret = fgets(linebuffer, 100, fp);
      if(ret == NULL)
      {
        break;
      }
      sscanf(linebuffer, "%[^=]=%[^=]", buffer1,buffer2);
      if(!strcmp(name, buffer1))
      {
         buffer_size=strlen(buffer2);
         if(buffer_size<size)//by sunzhguy should by fix pass the size to judge
         	{
	          memcpy(result,buffer2,buffer_size);
			  result[buffer_size-1]='\0';//remove the \n
			  flag = 1;
			   break;
         	}
		 
      }
      memset(buffer1,0,sizeof(buffer1));
      memset(buffer2,0,sizeof(buffer2));
      memset(linebuffer,0,sizeof(linebuffer));
  }
  fclose(fp);
  if(flag != 1)
  	return 1;
  return 0;
}
int ChangeFile_Info(char *name,char *writedata)
{
  int i;
  uint8_t flag = 0,cnt = 0;
  struct info_t info[12];
  char linebuffer[256] = {0};
  char buffer[80];
  FILE *fp = fopen(STR_NETWORKCFG_FILE, "r");
  if(fp == NULL)
  {
      //printf("open error");
      return 1;
  }
  for(i = 0 ; i < 12 ; i++)
  {
    char *ret = fgets(linebuffer, 256, fp);
    if(ret == NULL)
    {
      break;
    }
    sscanf(linebuffer, "%[^=]=%[^=]", info[i].name,info[i].data);
    if(!strcmp(info[i].name, name))
    {
      memcpy(info[i].data,writedata,strlen(writedata));
      info[i].data[strlen(writedata)] = '\n';
      info[i].data[strlen(writedata)+1] = '\0';
	  flag = 1;
    }
    memset(linebuffer,0,sizeof(linebuffer));
    cnt++;
  }
  fclose(fp);
  if(flag)
  {
  	fp = fopen(STR_NETWORKCFG_FILE,"w");
  	if(fp == NULL)
  	{
      	return 1;
  	}
  	for(i = 0 ; i < cnt ; i++)
  	{
    	memset(buffer,0,sizeof(buffer));
    	sprintf(buffer,"%s=%s",info[i].name,info[i].data);
    	fprintf(fp,"%s",buffer);
  	}
  	fclose(fp);
  }
  else
  	return 1;
  return 0;
}
int CutStr_ToHex(char *data, int flag ,uint8_t *hexdata)
{
	int i = 0;
	switch(flag)
	{
		case 0:
		{
			i = sscanf(data,"%x",&hexdata[0]);
			//printf("hexdata:%02x\n",hexdata[0]);
			break;
		}
		case 1:
		{
			char *ptr,*p;
			ptr = strtok_r(data,":",&p);
			while(ptr != NULL && (sscanf(ptr,"%x",&hexdata[i]) > 0))
			{
				//printf("ptr:%s--->%02x\n",ptr,hexdata[i]);
				i++;
				ptr = strtok_r(NULL,":", &p);  
			}
			break;
		}
		case 2:
		{
			char *ptr,*p;
			ptr = strtok_r(data,".",&p);
			while(ptr != NULL && (sscanf(ptr,"%d",&hexdata[i]) > 0))
			{
				//printf("ptr:%s--->%02x\n",ptr,hexdata[i]);
				i++;
				ptr = strtok_r(NULL,".", &p);  
			}
			break;
		}
		default:
			break;
	}
	return i;
}
int Info_Form_Hex(char *name, uint8_t *hexdata)
{
	char tmp[40]={0};
	int flag;
	int ret = GetFile_Info(name,tmp,40);
	if(ret != 0)
		goto err1;
	if(strcmp(name,"DHCP") == 0)
		flag = 0;
	else if(strcmp(name,"MAC") == 0)
		flag = 1;
	else 
		flag = 2;
	ret = CutStr_ToHex(tmp,flag,hexdata);
	if(ret <= 0)
		goto err1;
	return ret;
err1:
	return -1;
}
/*获取DHCP MAC IP内容转换为hex由485发送-----专用函数*/
int PackageHex_Senddata(uint8_t *senddata,char *path)
{
	uint8_t tmp[40]={0};
	int tmp_len,length;
	length = Info_Form_Hex("DHCP",tmp);
	if(length == -1)
		goto err1;
	memcpy(senddata,tmp,length);
	tmp_len = length;
	memset(tmp,0,sizeof(tmp));
	length = Info_Form_Hex("IP",tmp);
	if(length == -1)
		goto err1;
	memcpy(senddata+tmp_len,tmp,length);
	tmp_len = tmp_len + length;
	memset(tmp,0,sizeof(tmp));
	length = Info_Form_Hex("MAC",tmp);
	if(length == -1)
		goto err1;
	memcpy(senddata+tmp_len,tmp,length);
	tmp_len = tmp_len + length;
	return tmp_len;
err1:
	return -1;
}