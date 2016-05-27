/*
 Copyright 2003, 2016 Jeff Verkoeyen. All Rights Reserved.

 Licensed under the Apache License, Version 2.0 (the "License");
 you may not use this file except in compliance with the License.
 You may obtain a copy of the License at

 http://www.apache.org/licenses/LICENSE-2.0

 Unless required by applicable law or agreed to in writing, software
 distributed under the License is distributed on an "AS IS" BASIS,
 WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 See the License for the specific language governing permissions and
 limitations under the License.
 */

#include "Win32Window.h"
Win32Window Window;
#include "Logger.h"
Logger Log;
#include "resource.h"
#include <windows.h>
#include <string.h>
#include <commctrl.h>

enum { MAIN, ADD, EDIT, ABOUT, NUMWINDOWS };

HWND hWnd[NUMWINDOWS];

char GlobalDir[512];

struct Items
{
	float price;
	char name[255];
	int quantity;
};

Items *Item;

int Selected=-1;

int NumItems=0;
int MaxItems=0;

void SaveToFile()
{
	if(NumItems<=0)
		return;

	FILE* file;

	file=fopen("savefile.grg","wb");

	fwrite(&NumItems,sizeof(int),1,file);

	for(int a=0;a<NumItems;a++)
	{
		fwrite(&Item[a].price,sizeof(float),1,file);
		fwrite(&Item[a].name,sizeof(char)*255,1,file);
		fwrite(&Item[a].quantity,sizeof(int),1,file);
	}

	fclose(file);
}

void ReadFromFile()
{
	FILE* file;

	file=fopen("savefile.grg","rb");

	if(file)
	{
		fread(&NumItems,sizeof(int),1,file);

		if(Item)
			delete [] Item;

		Item=NULL;

		Item=new Items[NumItems];

		for(int a=0;a<NumItems;a++)
		{
			fread(&Item[a].price,sizeof(float),1,file);
			fread(&Item[a].name,sizeof(char)*255,1,file);
			fread(&Item[a].quantity,sizeof(int),1,file);
		}

		fclose(file);

		Window.Error("Notice",MB_OK,"File successfully restored.");
	}
}

void WriteToFile()
{
	ofstream file;

	file.open("Sales.htm",ios::out | ios::trunc);

	if(file.is_open())
	{
		file << "<html>" << endl;
		file << " <head>" << endl;
		file << "  <title>" << endl;
		file << "   GarageSale Sales" << endl;
		file << "  </title>" << endl;
		file << " </head>" << endl;
		file << " <body>" << endl;
		file << "  <table width='100%' border=2>" << endl;
		file << "   <tr>" << endl;
		file << "    <td>" << endl;
		file << "     <b>" << endl;
		file << "      Name" << endl;
		file << "     </b>" << endl;
		file << "    </td>" << endl;
		file << "    <td>" << endl;
		file << "     <b>" << endl;
		file << "      Price" << endl;
		file << "     </b>" << endl;
		file << "    </td>" << endl;
		file << "    <td>" << endl;
		file << "     <b>" << endl;
		file << "      Quantity" << endl;
		file << "     </b>" << endl;
		file << "    </td>" << endl;
		file << "    <td>" << endl;
		file << "     <b>" << endl;
		file << "      Total" << endl;
		file << "     </b>" << endl;
		file << "    </td>" << endl;
		file << "   </tr>" << endl;

		float Total=0;
		int TotalItems=0;

		for(int a=0;a<NumItems;a++)
		{
			file << "   <tr>" << endl;
			file << "    <td>" << endl;
			file << "     " << Item[a].name << endl;
			file << "    </td>" << endl;
			file << "    <td>" << endl;
			file << "     $" << Item[a].price << endl;
			file << "    </td>" << endl;
			file << "    <td>" << endl;
			file << "     " << Item[a].quantity << endl;
			TotalItems+=Item[a].quantity;
			file << "    </td>" << endl;
			file << "    <td>" << endl;
			file << "     $" << (float)Item[a].quantity*Item[a].price << endl;
// Debug	Window.Error("blah",0,"%d %f %f",Item[a].quantity,Item[a].price,(float)Item[a].quantity*Item[a].price);
			Total+=(float)Item[a].quantity*Item[a].price;
			file << "    </td>" << endl;
			file << "   </tr>" << endl;
		}

		file << "  </table>" << endl;
		file << "  <h3> Details </h3>" << endl;
		file << "  Number of Items Sold: " << TotalItems << "<br>" << endl;
		file << "  Total Price: $" << Total << "<br>" << endl;
		file << " </body>" << endl;
		file << "</html>" << endl;

		file.close();

		char temp[1024]="Explorer.exe ";

		int blah=strlen(temp);

		temp[blah]=34;
		temp[blah+1]='\0';

		strcat(temp,GlobalDir);
		strcat(temp,"\\Sales.htm");

		blah=strlen(temp);
		temp[blah]=34;
		temp[blah+1]='\0';
		system(temp);
	}
}

void UpdateListBox()
{
	HWND temp=GetDlgItem(hWnd[EDIT],IDC_VALUES);
	SendMessage(temp,LB_RESETCONTENT,NULL,NULL);
	for(int a=0;a<NumItems;a++)
		SendMessage(temp,LB_ADDSTRING,NULL,(LPARAM)Item[a].name);

	EnableWindow(GetDlgItem(hWnd[EDIT],IDC_EDIT),false);
	EnableWindow(GetDlgItem(hWnd[EDIT],IDC_REMOVE),false);

	temp=GetDlgItem(hWnd[EDIT],IDC_VALUE);
	SetWindowText(temp,"");

	temp=GetDlgItem(hWnd[EDIT],IDC_PRICE);
	SetWindowText(temp,"");

	temp=GetDlgItem(hWnd[EDIT],IDC_QUANTITY);
	SetWindowText(temp,"");
}

BOOL CALLBACK EditProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	Window.UpdateWindowDetails(msg,wParam,lParam);
    switch(msg)
    {
	case WM_MOVE:
		InvalidateRect(hwnd,NULL,true);
		UpdateWindow(hwnd);
		return true;
		break;
    case WM_CLOSE:
	case WM_DESTROY:
		EndDialog(hwnd,0);
		hWnd[EDIT]=NULL;
		return true;
		break;
	case WM_COMMAND:
		switch(LOWORD(wParam))
		{
		case IDC_EDIT:
			{
				char buff[255];
				GetDlgItemText(hwnd,IDC_PRICE,buff,255);
				Item[Selected].price=(float)atof(buff);

				GetDlgItemText(hwnd,IDC_VALUE,buff,255);
				strcpy(Item[Selected].name,buff);

				GetDlgItemText(hwnd,IDC_QUANTITY,buff,255);
				Item[Selected].quantity=atoi(buff);

				UpdateListBox();
				return true;
			}
			break;
		case IDC_DONE:
			EndDialog(hwnd,0);
			hWnd[EDIT]=NULL;
			return true;
			break;
		case IDC_CLEAR:
			if(MessageBox(hWnd[EDIT],"Are you sure that you want to clear the whole board?", "Clear board?",MB_YESNO|MB_ICONQUESTION)==IDYES)
			{
				SendMessage(GetDlgItem(hWnd[EDIT],IDC_VALUES),LB_RESETCONTENT,NULL,NULL);

				NumItems=0;

				EndDialog(hwnd,0);
				hWnd[EDIT]=NULL;
			}
			return true;
			break;
		case IDC_REMOVE:
			if(Selected==0)
			{
				if(NumItems==2)
					Item[0]=Item[1];
				else if(NumItems>2)
				{
					for(int a=0;a<NumItems-1;a++)
						Item[a]=Item[a+1];
				}
			}
			else if(Selected>0 && Selected<NumItems-1)
			{
				for(int a=Selected;a<NumItems-1;a++)
					Item[a]=Item[a+1];
			}
			NumItems--;
			if(NumItems<=0)
			{
				NumItems=0;
				EndDialog(hwnd,0);
				hWnd[EDIT]=NULL;
			}
			else
			{
				UpdateListBox();
			}
			return true;
			break;
		case IDC_VALUES:
			switch(HIWORD(wParam))
			{
			case LBN_SELCHANGE:
				{
					HWND temp=GetDlgItem(hWnd[EDIT],IDC_VALUES);
					Selected=SendMessage(temp,LB_GETCURSEL,NULL,NULL);

					EnableWindow(GetDlgItem(hWnd[EDIT],IDC_EDIT),true);
					EnableWindow(GetDlgItem(hWnd[EDIT],IDC_REMOVE),true);

					temp=GetDlgItem(hWnd[EDIT],IDC_VALUE);
					SetWindowText(temp,Item[Selected].name);

					temp=GetDlgItem(hWnd[EDIT],IDC_PRICE);
					char value[255];
					sprintf(value,"%f",Item[Selected].price);
					SetWindowText(temp,value);

					temp=GetDlgItem(hWnd[EDIT],IDC_QUANTITY);
					sprintf(value,"%d",Item[Selected].quantity);
					SetWindowText(temp,value);

					UpdateWindow(hwnd);
					return true;
				}
				break;
			}

		}
		break;
	}
	return FALSE;
}

BOOL CALLBACK AddProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	Window.UpdateWindowDetails(msg,wParam,lParam);
    switch(msg)
    {
	case WM_MOVE:
		InvalidateRect(hwnd,NULL,true);
		UpdateWindow(hwnd);
		return true;
		break;
    case WM_CLOSE:
	case WM_DESTROY:
		EndDialog(hwnd,0);
		hWnd[ADD]=NULL;
		return true;
		break;
	case WM_COMMAND:
		switch(LOWORD(wParam))
		{
		case IDC_OK:
			NumItems++;
			if(NumItems>MaxItems)
			{
				MaxItems=NumItems;
				if(NumItems==1)
					Item=new Items[NumItems];
				else
					Item=(Items*)realloc(Item,sizeof(Items)*NumItems);
			}
			char buff[255];
			GetDlgItemText(hwnd,IDC_PRICE,buff,255);
			Item[NumItems-1].price=(float)atof(buff);

			GetDlgItemText(hwnd,IDC_VALUE,buff,255);
			strcpy(Item[NumItems-1].name,buff);

			GetDlgItemText(hwnd,IDC_QUANTITY,buff,255);
			Item[NumItems-1].quantity=atoi(buff);

			if(hWnd[EDIT]!=NULL)
				UpdateListBox();

			EndDialog(hwnd,0);
			hWnd[ADD]=NULL;
			return true;
			break;
		case IDC_CANCEL:
			EndDialog(hwnd,0);
			hWnd[ADD]=NULL;
			return true;
			break;
		}
		break;
	}
	return FALSE;
}

BOOL CALLBACK AboutProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	Window.UpdateWindowDetails(msg,wParam,lParam);
    switch(msg)
    {
	case WM_MOVE:
		InvalidateRect(hwnd,NULL,true);
		UpdateWindow(hwnd);
		return true;
		break;
    case WM_CLOSE:
	case WM_DESTROY:
		EndDialog(hwnd,0);
		hWnd[ABOUT]=NULL;
		return true;
		break;
	case WM_COMMAND:
		switch(LOWORD(wParam))
		{
		case IDC_OK:
			EndDialog(hwnd,0);
			hWnd[ABOUT]=NULL;
			return true;
			break;
		}
		break;
	}
	return FALSE;
}

BOOL CALLBACK MainProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	Window.UpdateWindowDetails(msg,wParam,lParam);
    switch(msg)
    {
	case WM_MOVE:
		InvalidateRect(hwnd,NULL,true);
		UpdateWindow(hwnd);
		return true;
		break;
    case WM_CLOSE:
	case WM_DESTROY:
		EndDialog(hwnd,0);
		PostQuitMessage(0);
		return true;
		break;
	case WM_COMMAND:
		switch(LOWORD(wParam))
		{
		case IDC_ABOUT:
			if(hWnd[ABOUT]==NULL)
			{
				hWnd[ABOUT]=CreateDialog(Window.GetHINSTANCE(),MAKEINTRESOURCE(IDD_ABOUT),Window.GetHWND(),(DLGPROC)AboutProc);
				ShowWindow(hWnd[ABOUT],SW_SHOW);
				UpdateWindow(hWnd[ABOUT]);
			}
			return true;
			break;
		case IDC_ADD:
			if(hWnd[ADD]==NULL)
			{
				hWnd[ADD]=CreateDialog(Window.GetHINSTANCE(),MAKEINTRESOURCE(IDD_ADD),Window.GetHWND(),(DLGPROC)AddProc);
				ShowWindow(hWnd[ADD],SW_SHOW);
				UpdateWindow(hWnd[ADD]);
			}
			return true;
			break;
		case IDC_EDIT:
			{
				if(NumItems>=1)
				{
					if(hWnd[EDIT]==NULL)
					{
						hWnd[EDIT]=CreateDialog(Window.GetHINSTANCE(),MAKEINTRESOURCE(IDD_EDIT),Window.GetHWND(),(DLGPROC)EditProc);
						UpdateListBox();
						ShowWindow(hWnd[EDIT],SW_SHOW);
						UpdateWindow(hWnd[EDIT]);
					}
				}
				else
					MessageBox(hWnd[MAIN],"Nothing to edit.","Error",MB_OK);
				return true;
			}
			break;
		case IDC_WRITE:
			if(NumItems>=1)
				WriteToFile();
			else
				MessageBox(hWnd[MAIN],"Can't write to file until there is at least one value added.","Error",MB_OK);
			return true;
			break;
		case IDC_SAVE:
			if(NumItems>=1)
				SaveToFile();
			else
				MessageBox(hWnd[MAIN],"Can't save to file until there is at least one value added.","Error",MB_OK);
			return true;
			break;
		}
		break;
	case WM_TIMER:
		if(SendMessage(GetDlgItem(hWnd[MAIN],IDC_AUTO_ON),BM_GETCHECK,NULL,NULL)==BST_CHECKED)
		{
			if(NumItems>0)
				SaveToFile();
		}
		break;
	}
	return FALSE;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	Window.UpdateWindowDetails(msg,wParam,lParam);

	switch(msg)
	{
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	case WM_CLOSE:
		Window.KillWin();
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hwnd, msg, wParam, lParam);
	}

	return 0;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	MSG Msg;
	GetCurrentDirectory(512,GlobalDir);
	InitCommonControls();

	Window.InitWindow(&Log);

	if(!Window.CreateWin("GarageSale Program",800,600,hInstance,SW_SHOWMAXIMIZED))
		return FALSE;

	ReadFromFile();

	hWnd[MAIN]=CreateDialog(Window.GetHINSTANCE(),MAKEINTRESOURCE(IDD_MAIN),Window.GetHWND(),(DLGPROC)MainProc);

	SetTimer(hWnd[MAIN],WM_TIMER,120000,NULL);

	SendMessage(GetDlgItem(hWnd[MAIN],IDC_AUTO_ON),BM_SETCHECK,BST_CHECKED,NULL);

	ShowWindow(hWnd[MAIN],SW_SHOW);
	UpdateWindow(hWnd[MAIN]);
	while(GetMessage(&Msg, NULL, 0, 0) > 0)
	{
		TranslateMessage(&Msg);
		DispatchMessage(&Msg);
	}

	return Msg.wParam;
}