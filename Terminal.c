#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<math.h>
#include<conio.h>
#include<windows.h>

void mkdir(int cur_block,char param[]);
int gotoBlock(int block);
int existsDir(int cur_block,char param[],int *linePointer);
int existsFile(int cur_block,char param[],int *size,int *linePointer);
void vi(int cur_block,char param[]);
void FATupdate(int block,int next);
int blockSearch();
int toDec(char tmp[]);
void getCursorXY(int *x,int *y);
void gotoxy(int x,int y);
void textEditor(char file[],int mode,int size);
void readData(int readBlock,char file[]);
void writeData(int writeBlock,char file[]);
int nextBlock(int writeBlock);
void ls(int cur_block);
int cd(int cur_block,char param[]);
void rm(int cur_block,char param[]);
void formatBlock(int dataBlock);
void rd(int cur_block,char param[]);
void ren(int cur_block,char param[],char param2[]);
void rendir(int cur_block,char param[],char param2[]);

int main()
{
	int cur_block=1,ch,i,size;
	char command[10],param[10],param2[10];
	char commands[10][10]={"mkdir","vi","ls","cd","rm","rmdir","ren","rendir","clrscr","exit"};
	int path[256],ipath=0;
	char Path[256][9];
	path[ipath]=cur_block;
	strcpy(Path[ipath],"Home");
	while(1)
	{
		int tmp,p;
		ch=-1;
		printf(">%s",Path[0]);
		for(p=1;p<=ipath;p++)
		printf("\\%s",Path[p]);
		printf("~");
		scanf("%s",command);
		for(i=0;i<10;++i)
		if(!strcmp(command,commands[i]))
		{
			ch=i;
			break;
		}
		
		if(ch==-1)
		{
			printf("Invalid command!");
			fflush(stdin);
		}
		else
		switch(ch)
		{
			case 0: scanf("%s",param);
					mkdir(cur_block,param);
					fflush(stdin);
					break;
			case 1: scanf("%s",param);
					fflush(stdin);
					vi(cur_block,param);
					fflush(stdin);
					break;
			case 2: ls(cur_block);
					fflush(stdin);
					break;
			case 3: scanf("%s",param);
					if(!strcmp(param,".."))
					{
						if(cur_block==1)
						{
							printf("Already at root directory!\n");
							fflush(stdin);
							break;
						}
						cur_block=path[--ipath];
						fflush(stdin);
						break;
					}
					tmp=cd(cur_block,param);
					if(tmp)
					{
						path[++ipath]=tmp;
						cur_block=tmp;
						strcpy(Path[ipath],param);
					}
					fflush(stdin);
					break;
			case 4:	scanf("%s",param);
					rm(cur_block,param);
					fflush(stdin);
					break;
			case 5: scanf("%s",param);
					rd(cur_block,param);
					fflush(stdin);
					break;
			case 6: scanf("%s %s",param,param2);
					ren(cur_block,param,param2);
					fflush(stdin);
					break;
			case 7: scanf("%s %s",param,param2);
					rendir(cur_block,param,param2);
					fflush(stdin);
					break;
			case 8: system("cls");
					fflush(stdin);
					break;
			case 9: exit(0);
					break;
		}
	}
}

void ren(int cur_block,char param[],char param2[])
{
	int size,linePointer,fileBlock,i;
	FILE *fp;
	char tmp[5];
	fileBlock=existsFile(cur_block,param,&size,&linePointer);
	if(strlen(param2)>8)
	{
		printf("New file-name length exceeds permitted length.");
		return;
	}
	if(!fileBlock)
	{
		printf("File: %s doesn't exist.\n",param);
		return;
	}
	fp=fopen("disk.txt","r+");
	fseek(fp,linePointer,SEEK_SET);
	fseek(fp,5,SEEK_CUR);
	for(i=0;i<strlen(param2);i++)
	{
		char hex[3];
		itoa(param2[i],hex,16);
		fprintf(fp,"0x%c%c ",hex[0],hex[1]);
	}
	for(i;i<8;i++)
	fprintf(fp,"%s ","0x00");
	fclose(fp);	
}

void rendir(int cur_block,char param[],char param2[])
{
	int linePointer,dirBlock,i;
	FILE *fp;
	char tmp[5];
	dirBlock=existsDir(cur_block,param,&linePointer);
	if(strlen(param2)>8)
	{
		printf("New Directory-name length exceeds permitted length.");
		return;
	}
	if(!dirBlock)
	{
		printf("Directory: %s doesn't exist.\n",param);
		return;
	}
	fp=fopen("disk.txt","r+");
	fseek(fp,linePointer,SEEK_SET);
	fseek(fp,5,SEEK_CUR);
	for(i=0;i<strlen(param2);i++)
	{
		char hex[3];
		itoa(param2[i],hex,16);
		fprintf(fp,"0x%c%c ",hex[0],hex[1]);
	}
	for(i;i<8;i++)
	fprintf(fp,"%s ","0x00");
	fclose(fp);	
}

void rd(int cur_block,char param[])
{
	FILE *fp;
	int dirBlock,block,linePointer,i,pos;
	char tmp[5];
	fp=fopen("disk.txt","r+");
	dirBlock=existsDir(cur_block,param,&linePointer);
	if(!dirBlock)
	{
		printf("Directory: %s doesn't exist.\n",param);
		fclose(fp);
		return;
	}
	fseek(fp,linePointer,SEEK_SET);
	for(i=0;i<16;i++)
	fprintf(fp,"%s ","0xxx");
	rewind(fp);
	FATupdate(dirBlock,0);
	block=gotoBlock(dirBlock);
	while(block--)
	while(fgetc(fp)!='\n');
	for(i=0;i<32;i++)
	{
		char name[9];
		fscanf(fp,"%s ",tmp);
		if(!strcmp(tmp,"0x01"))
		{
			int i=7,j=0;
			fscanf(fp,"%s ",tmp);
			while(strcmp(tmp,"0x00") && i--)
			{
				name[j++]=toDec(tmp);
				fscanf(fp,"%s ",tmp);
			}
			name[j]='\0';
			while(i--)
			fscanf(fp,"%s ",tmp);
			pos=ftell(fp);
			fclose(fp);
			rd(dirBlock,name);
			fp=fopen("disk.txt","r+");
			fseek(fp,pos,SEEK_SET);
			while(fgetc(fp)!='\n');
		}
		else if(!strcmp(tmp,"0x02"))
		{
			int i=7,j=0;
			fscanf(fp,"%s ",tmp);
			while(strcmp(tmp,"0x00") && i--)
			{
				name[j++]=toDec(tmp);
				fscanf(fp,"%s ",tmp);
			}
			name[j]='\0';
			while(i--)
			fscanf(fp,"%s ",tmp);
			pos=ftell(fp);
			fclose(fp);
			rm(dirBlock,name);
			fp=fopen("disk.txt","r+");
			fseek(fp,pos,SEEK_SET);
			while(fgetc(fp)!='\n');
		}
		else
		while(fgetc(fp)!='\n');
	}
	printf("Directory: %s deleted.\n",param);
	fclose(fp);
}

void rm(int cur_block,char param[])
{
	FILE *fp;
	int block,i,linePointer,size,dataBlock,nBlock;
	char tmp[5];
	fp=fopen("disk.txt","r+");
	dataBlock=existsFile(cur_block,param,&size,&linePointer);
	if(!dataBlock)
	{
		printf("File: %s doesn't exist.\n",param);
		fclose(fp);
		return;
	}
	fseek(fp,linePointer,SEEK_SET);
	for(i=0;i<16;++i)
	fprintf(fp,"%s ","0xxx");
	
	formatBlock(dataBlock);
	nBlock=nextBlock(dataBlock);
	FATupdate(dataBlock,0);
	dataBlock=nBlock;
	while(dataBlock)
	{
		nBlock=nextBlock(dataBlock);
		formatBlock(dataBlock);
		FATupdate(dataBlock,0);
		dataBlock=nBlock;
	}
	
	printf("File: %s deleted successfully.\n",param);
	fclose(fp);
}

void formatBlock(int dataBlock)
{
	int block,pos,i,j;
	FILE *fp=fopen("disk.txt","r+");
	block=gotoBlock(dataBlock);
	while(block--)
	while(fgetc(fp)!='\n');
	pos=ftell(fp);
	fseek(fp,pos,SEEK_SET);
	for(i=1;i<33;i++)
	{
		for(j=1;j<17;j++)
		fprintf(fp,"%s ","0xxx");
		fprintf(fp,"%s","\r\n");
	}
	fclose(fp);
}

int cd(int cur_block,char param[])
{
	int block,dirBlock,linePointer;
	dirBlock=existsDir(cur_block,param,&linePointer);
	if(!dirBlock)
	{
		printf("Directory:%s doesn't exist!\n",param);
		return 0;
	}
	return dirBlock;
}

void ls(int cur_block)
{
	FILE *fp;
	int block,i;
	char tmp[5],name[9]="";
	fp=fopen("disk.txt","r");
	block=gotoBlock(cur_block);
	while(block--)
	while(fgetc(fp)!='\n');
	for(i=1;i<33;i++)
	{
		fscanf(fp,"%s ",tmp);
		if(!strcmp(tmp,"0x01"))
		{
			int i=7,j=0;
			fscanf(fp,"%s ",tmp);
			while(strcmp(tmp,"0x00") && i--)
			{
				name[j++]=toDec(tmp);
				fscanf(fp,"%s ",tmp);
			}
			name[j]='\0';
			while(fgetc(fp)!='\n');
			printf("%s\t\tDirectory\n",name);
		}
		else if(!strcmp(tmp,"0x02"))
		{
			int i=7,j=0,size;
			fscanf(fp,"%s ",tmp);
			while(strcmp(tmp,"0x00") && i--)
			{
				name[j++]=toDec(tmp);
				fscanf(fp,"%s ",tmp);
			}
			name[j]='\0';
			while(i--)
			fscanf(fp,"%s ",tmp);
			fscanf(fp,"%s ",tmp);
			fscanf(fp,"%s ",tmp);
			size=toDec(tmp);
			fscanf(fp,"%s ",tmp);
			size=size*16*16+toDec(tmp);
			printf("%s\t\tFile\t\t%d bytes\n",name,size);
			while(fgetc(fp)!='\n');
		}
		else
		while(fgetc(fp)!='\n');
	}
	if(!strlen(name))
	printf("Directory is empty!\n");
	fclose(fp);
}

int gotoBlock(int block)
{
	return 1+34*(block);
}

void mkdir(int cur_block,char param[])
{
	FILE *fp;
	char tmp[5];
	int block,i,pos,linePointer;
	fp=fopen("disk.txt","r+");
	if(strlen(param)>8)
	{
		printf("Directory name exceeds permitted length.");
		fclose(fp);
		return;
	}
	if(existsDir(cur_block,param,&linePointer))
	{
		printf("Directory already exists!\n");
		fclose(fp);
		return;
	}
	
	block=gotoBlock(cur_block);
	while(block--)
	while(fgetc(fp)!='\n');
	for(i=1;i<33;i++)
	{
		pos=ftell(fp);
		fscanf(fp,"%s",tmp);
		if(!strcmp(tmp,"0xxx"))
		break;
		else
		while(fgetc(fp)!='\n');
	}
	
	if(i==33)
	{
		printf("Present working directory is full and cannot accomodate more sub-directories. Please delete or move files to free space.");
		fclose(fp);
		return;
	}
	
	
	block=blockSearch();
	if(block==-1)
	{
		printf("Disk Memory is full. Delete data to free space.");
		fclose(fp);
		return;
	}
	fseek(fp,pos,SEEK_SET);
	fprintf(fp,"%s ","0x01");
	
	for(i=0;i<strlen(param);i++)
	{
		char hex[3];
		itoa(param[i],hex,16);
		fprintf(fp,"0x%c%c ",hex[0],hex[1]);
	}
	for(i;i<8;i++)
	fprintf(fp,"%s ","0x00");
	char hex[3];
	itoa(block,hex,16);
	if(strlen(hex)>1)
	fprintf(fp,"0x%c%c ",hex[0],hex[1]);
	else
	fprintf(fp,"0x0%c ",hex[0]);
	
	FATupdate(block,-1);
	printf("Directory created successfully\n");
	fclose(fp);
}

void FATupdate(int block,int next)
{
	FILE *fp;
	int i,pos;
	char add[5],hex[3];
	fp=fopen("disk.txt","r+");
	while(fgetc(fp)!='\n');
	for(i=0;i<block;i++)
	fscanf(fp,"%s ",add);
	pos=ftell(fp);
	fseek(fp,pos,SEEK_SET);
	if(next==-1)
	fprintf(fp,"%s ","0x00");
	else if(next==0)
	fprintf(fp,"%s ","0xxx");
	else
	{
		itoa(next,hex,16);
		if(strlen(hex)>1)
		fprintf(fp,"0x%c%c ",hex[0],hex[1]);
		else
		fprintf(fp,"0x0%c ",hex[0]);
	}
	fclose(fp);
}

int blockSearch()
{
	FILE *fp;
	int i,cnt=-1;
	char add[5];
	fp=fopen("disk.txt","r+");
	while(fgetc(fp)!='\n');
	while(cnt<256)
	{
		fscanf(fp,"%s",add);
		cnt++;
		if(!strcmp(add,"0xxx"))
		return cnt;		
	}
	return -1;
}

int existsDir(int cur_block,char param[],int *linePointer)
{
	FILE *fp;
	int block,i,lp;
	char tmp[5],name[9];
	fp=fopen("disk.txt","r+");
	block=gotoBlock(cur_block);
	while(block--)
	while(fgetc(fp)!='\n');
	for(i=1;i<33;i++)
	{
		lp=ftell(fp);
		fscanf(fp,"%s ",tmp);
		if(!strcmp(tmp,"0x01"))
		{
			int i=7,j=0;
			fscanf(fp,"%s ",tmp);
			while(strcmp(tmp,"0x00") && i--)
			{
				name[j++]=toDec(tmp);
				fscanf(fp,"%s ",tmp);
			}
			name[j]='\0';
			while(i--)
			fscanf(fp,"%s ",tmp);
			
			fscanf(fp,"%s ",tmp);
			if(!strcmp(name,param))
			{
				fclose(fp);
				*linePointer=lp;
				return toDec(tmp);
			}
			else
			while(fgetc(fp)!='\n');
		}
		else
		while(fgetc(fp)!='\n');
	}
	fclose(fp);
	return 0;
}

int existsFile(int cur_block,char param[],int *size,int *linePointer)
{
	FILE *fp;
	int block,i,lp;
	char tmp[5],name[9];
	fp=fopen("disk.txt","r+");
	block=gotoBlock(cur_block);
	while(block--)
	while(fgetc(fp)!='\n');
	block=0;
	for(i=1;i<33;i++)
	{
		lp=ftell(fp);
		fscanf(fp,"%s ",tmp);
		if(!strcmp(tmp,"0x02"))
		{
			int i=7,j=0;
			fscanf(fp,"%s ",tmp);
			while(strcmp(tmp,"0x00") && i--)
			{
				name[j++]=toDec(tmp);
				fscanf(fp,"%s ",tmp);
			}
			name[j]='\0';
			if(!strcmp(name,param))
			{
				while(i--)
				fscanf(fp,"%s ",tmp);
				fscanf(fp,"%s ",tmp);
				block=toDec(tmp);
				*linePointer=lp;
				break;
			}
		}
		else
		while(fgetc(fp)!='\n');
	}
	if(block)
	{
		fscanf(fp,"%s ",tmp);
		*size=toDec(tmp);
		fscanf(fp,"%s ",tmp);
		*size=(*size)*16*16+toDec(tmp);
		fclose(fp);
		return block;
	}
	fclose(fp);
	return 0;
}

int toDec(char tmp[])
{
	int i,dec=0;
	for(i=2;i<4;i++)
	{
		if(tmp[i]>47 && tmp[i]<58)
		dec+=(tmp[i]-48)*pow(16,3-i);
		else
		dec+=(tmp[i]-87)*pow(16,3-i);
	}
	return dec;
}

void vi(int cur_block,char param[])
{
	FILE *fp;
	char tmp[5];
	int block,i,pos,clusterSize,nextBlock,writeBlock,readBlock,size,linePointer;
	fp=fopen("disk.txt","r+");
	if(strlen(param)>8)
	{
		printf("Directory name exceeds permitted length.");
		return;
	}
	readBlock=existsFile(cur_block,param,&size,&linePointer);
	if(readBlock)
	{
		char file[size];
		readData(readBlock,file);
		printf("%s.txt >\n");
		textEditor(file,1,size);
		printf("\n");
		writeData(readBlock,file);
		return;
	}
	printf("\nEnter file size in bytes(Not more than 2048 bytes):");
	scanf("%d",&size);
	char file[size];
	if(size>2048)
	{
		printf("Cannot create file with size greater than 2048 bytes!\n");
		return;
	}
	if(size==0)
	{
		printf("File Size cannot be zero.");
		return;
	}
	block=gotoBlock(cur_block);
	while(block--)
	while(fgetc(fp)!='\n');
	for(i=1;i<33;i++)
	{
		pos=ftell(fp);
		fscanf(fp,"%s ",tmp);
		if(!strcmp(tmp,"0xxx"))
		break;
		else
		while(fgetc(fp)!='\n');
	}
	if(i==33)
	{
		printf("Present working directory is full and cannot accomodate more files. Please delete or move files to free space.");
		return;
	}
	fseek(fp,pos,SEEK_SET);
	fprintf(fp,"%s ","0x02");
	for(i=0;i<strlen(param);i++)
	{
		char hex[3];
		itoa(param[i],hex,16);
		fprintf(fp,"0x%c%c ",hex[0],hex[1]);
	}
	for(i;i<8;i++)
	fprintf(fp,"%s ","0x00");
	
	char hex[4];
	block=blockSearch();
	writeBlock=block;
	itoa(block,hex,16);
	if(strlen(hex)>1)
	fprintf(fp,"0x%c%c ",hex[0],hex[1]);
	else
	fprintf(fp,"0x0%c ",hex[0]);
	
	itoa(size,hex,16);
	if(strlen(hex)==3)
	fprintf(fp,"0x0%c 0x%c%c ",hex[0],hex[1],hex[2]);
	else if(strlen(hex)==2)
	fprintf(fp,"0x00 0x%c%c ",hex[0],hex[1]);
	else
	fprintf(fp,"0x00 0x0%c ",hex[0]);
	
	clusterSize=ceil(size/512.0);
	while(--clusterSize)
	{
		FATupdate(block,-1);
		nextBlock=blockSearch();
		FATupdate(block,nextBlock);
		block=nextBlock;
	}
	FATupdate(block,-1);
	
	printf("\nType now\n");
	textEditor(file,0,size);
	printf("\n");
	writeData(writeBlock,file);
	printf("\n");
	fclose(fp);
}

void textEditor(char file[],int mode,int size)
{
	int x,y,xi[size+1],i,j=-1;
	if(mode)
	{
		for(i=0;file[i]!='\0';i++)
		{
			if(file[i]==0x0d)
			{
				getCursorXY(&x,&y);
				xi[++j]=x;
				printf("\n");
			}
			else
			printf("%c",file[i]);
		}
		i--;
	}
	else
	i=-1;
	while(1)
	{
		if(kbhit())
		{
			unsigned char ch=getch();
			if(ch==0x1b)
			break;
			if(ch==0x08 && i>-1)
			{ 
				getCursorXY(&x,&y);
				if(x==0)
				{
					gotoxy(xi[j--],y-1);
					file[i--]='\0';
				}
				file[i--]='\0'; printf("\b \b");
			}
			else if(i==size-2)
			printf("\a");
			else if(ch==0x0d)
			{ 
				file[++i]=ch; 
				getCursorXY(&x,&y);
				xi[++j]=x;
				printf("\n"); 
			}
			else if(ch>31 && ch<127)
			{ file[++i]=ch; putchar(ch); }
		}
	}
	file[++i]='\0';
}

void readData(int readBlock,char file[])
{
	FILE *fp;
	int block,i=0,j,fl;
	char add[5];
	while(1)
	{
		fp=fopen("disk.txt","r+");
		block=gotoBlock(readBlock);
		while(block--)
		while(fgetc(fp)!='\n');
		for(j=0;j<512;j++)
		{
			fscanf(fp,"%s ",add);
			if(!strcmp(add,"0x03"))
			{
				fl=1; break;
			}
			file[i++]=toDec(add);
		}
		readBlock=nextBlock(readBlock);
		fclose(fp);
		if(fl==1)
		{ file[i]='\0'; break; }
	}
}

void writeData(int writeBlock,char file[])
{
	FILE *fp;
	int block,i=0,j,pos,size;
	char hex[3];
	size=strlen(file);
	while(size>512)
	{
		fp=fopen("disk.txt","r+");
		block=gotoBlock(writeBlock);
		while(block--)
		while(fgetc(fp)!='\n');
		pos=ftell(fp);
		fseek(fp,pos,SEEK_SET);
		for(j=0;j<512;j++)
		{
			itoa(file[i++],hex,16);
			if(strlen(hex)==2)
			fprintf(fp,"0x%c%c ",hex[0],hex[1]);
			else
			fprintf(fp,"0x0%c ",hex[0]);
		}
		size-=512;
		writeBlock=nextBlock(writeBlock);
		fclose(fp);
	}
	fp=fopen("disk.txt","r+");
	block=gotoBlock(writeBlock);
	while(block--)
	while(fgetc(fp)!='\n');
	pos=ftell(fp);
	fseek(fp,pos,SEEK_SET);
	while(size--)
	{
		itoa(file[i++],hex,16);
		if(strlen(hex)==2)
		fprintf(fp,"0x%c%c ",hex[0],hex[1]);
		else
		fprintf(fp,"0x0%c ",hex[0]);
	}
	fprintf(fp,"%s ","0x03");
	fclose(fp);
}

int nextBlock(int writeBlock)
{
	FILE *fp;
	char add[5];
	fp=fopen("disk.txt","r");
	while(fgetc(fp)!='\n');
	while(writeBlock--)
	fscanf(fp,"%s ",add);
	fscanf(fp,"%s ",add);
	fclose(fp);
	return toDec(add);
}

void getCursorXY(int *x,int *y)
{
	CONSOLE_SCREEN_BUFFER_INFO csbi;
    if(GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE),&csbi)) 
	{
        *x = csbi.dwCursorPosition.X;
        *y = csbi.dwCursorPosition.Y;
    }
}

void gotoxy(int x,int y)
{
	COORD pos={x,y};
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE),pos);
}