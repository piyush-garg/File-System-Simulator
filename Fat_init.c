#include<stdio.h>

int main()
{
	FILE *fp;
	int i,j,k,pos;
	char c;
	fp=fopen("disk.txt","r+");
	for(i=1;i<257;++i)
	{
		fprintf(fp,"%s","Block ");
		fprintf(fp,"%d",i-1);
		fprintf(fp,"%s","\r\n");
		for(j=1;j<33;++j)
		{
			for(k=1;k<17;++k)
			fprintf(fp,"%s ","0xxx");
			fprintf(fp,"%s","\r\n");
		}
		fprintf(fp,"%s","\r\n");
	}
	rewind(fp); 
	while(fgetc(fp)!='\n');
	pos=ftell(fp);
	fseek(fp,pos,SEEK_SET);
	
	fputs("0x01 0x01 ",fp);
	return 0;
}