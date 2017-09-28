// UGupdate.cpp: 定义控制台应用程序的入口点。
//

#include "stdafx.h"

#include <iostream>
#include <string>
#include <uf.h>
#include <uf_modl.h>
#include <uf_ps.h>
#include <uf_obj.h>
#include <uf_assem.h>
#include <uf_part.h>
#include <uf_modl_expressions.h>
using namespace std;
/*
功能:
用exp文件更新prt文件文，并将prt文件另存为x_t文件
调用方法:
system("you.prt   you.exp   you.x_t")
createprocess("you.prt"，"you.exp   d:\\you.x_t"...)
注意：
文件名一定要写全，在导出时会自动删除旧的文件否则可能
因为文件已经存在而导致导出x_t文件失败。

san,nuaa,202
　　　　　　　　　　　　　　　　　　　　visualsan@yahoo.cn
					2011.11.13
					*/
int  prt_save_as_xt(tag_t, string ps);
//引入lib文件，因人而异
#pragma  comment(lib,"C:\\Program Files\\Siemens\\NX 11.0\\UGOPEN\\libufun.lib")
#define VISUALSAN_NUAA_202_RUN(x)\
if(0!=x)\
{\
    char msg[133];\
    UF_get_fail_message( x,msg );\
    cout<<msg;\
    return -1;\
}

int  main(int argc, char **argv)
{

	//实现初始化ug api,否则其它ug函数是无法使用的
	VISUALSAN_NUAA_202_RUN(UF_initialize());

	//文件路径
	string  prt, ep;
	string  x_t;//x_t格式文件地址

				/*参数个数检查，
				注意用system("UG_update.exe xx.prt  yy.exp")调用时，
				默认第一个参数为exe地址，所以你的参数
				是第二个开始；用createprocess时，则参数和你传递的一样
				system("UG_update.exe xx.prt  yy.x_t")
				argv[0]=UG_update.exe argv[1]=xx.prt argv[2]=yy.exp  argv[3]=yy.x_t

				createprocess("UG_update.exe","xx.prt  yy.exp yy.x_t",....)
				argv[0]=xx.prt argv[1]=yy.exp argv[2]=yy.x_t*/

	if (argc<3)
	{
		cerr << "参数个数不足";
		return  -1;
	}
	//默认你是用createprocess创建线程的
	if (argc == 3)
	{
		prt = argv[0];  //prt文件
		ep = argv[1];  //exp文件
		x_t = argv[2]; //x_t文件
	}
	//否则是用system调用的
	else
	{
		prt = argv[1]; //prt文件
		ep = argv[2]; //exp文件
		x_t = argv[3]; //x_t文件
	}

	//打开模型文件
	UF_PART_load_status_t st;
	tag_t  prt_id;
	//打开prt文件
	VISUALSAN_NUAA_202_RUN(UF_PART_open(prt.c_str(), &prt_id, &st));
	//更新模型文件
	VISUALSAN_NUAA_202_RUN(UF_MODL_import_exp((char*)ep.c_str(), 0));
	//更新模型
	VISUALSAN_NUAA_202_RUN(UF_MODL_update());
	//写入文件
	VISUALSAN_NUAA_202_RUN(UF_PART_save());

	//另存为x_t文件
	if (-1 == prt_save_as_xt(prt_id, x_t))
		return -1;

	//关闭prt文件
	VISUALSAN_NUAA_202_RUN(UF_PART_close(prt_id, 1, 1));
	VISUALSAN_NUAA_202_RUN(UF_PART_free_load_status(&st));
	//退出前UF_terminate调用清理
	VISUALSAN_NUAA_202_RUN(UF_terminate());

	return 0;
}
int  prt_save_as_xt(tag_t body_tag, string ps)
{
	//没有后缀名时，加上后缀名
	if (std::string::npos == ps.find(".x_t"))
		ps += ".x_t";
	//引用代码:Ug2Ansys.cpp  李 响, 中国地质大学(北京) 2006.12.31
	uf_list_p_t body_list;
	// 获得装配树根事例root_part_occ, 当函数返回NULL_TAG时, 表明当前部件文件中没有装配(即单个部件)
	tag_t root_part_occ = UF_ASSEM_ask_root_part_occ(body_tag);
	VISUALSAN_NUAA_202_RUN(UF_MODL_create_list(&body_list));
	// 如果是单个部件
	if (root_part_occ == NULL_TAG)
	{
		tag_t object = NULL_TAG;
		int UF_body_type;
		int type;
		int subtype;
		do {
			VISUALSAN_NUAA_202_RUN(
				UF_OBJ_cycle_objs_in_part(body_tag, UF_solid_type, &object)
			);
			if (object != NULL_TAG)
			{
				VISUALSAN_NUAA_202_RUN(
					UF_OBJ_ask_type_and_subtype(object, &type, &subtype)
				);
				VISUALSAN_NUAA_202_RUN(
					UF_MODL_ask_body_type(object, &UF_body_type)
				);

				if (subtype != UF_solid_body_subtype)
					continue;
				if (UF_body_type == UF_MODL_SOLID_BODY)
				{
					VISUALSAN_NUAA_202_RUN(
						UF_MODL_put_list_item(body_list, object)
					);
					break;
				}
			}
		} while (1);

	}
	//如果是装配体
	else
	{
		tag_t obj = UF_ASSEM_ask_prototype_of_occ(root_part_occ);
		tag_t object = NULL_TAG;
		int UF_body_type;
		int type;
		int subtype;
		do
		{
			VISUALSAN_NUAA_202_RUN(
				UF_OBJ_cycle_objs_in_part(body_tag, UF_solid_type, &object));
			if (object != NULL_TAG)
			{
				VISUALSAN_NUAA_202_RUN(
					UF_OBJ_ask_type_and_subtype(object, &type, &subtype));
				// 判断body是否是一个Solid或Sheet
				VISUALSAN_NUAA_202_RUN(
					UF_MODL_ask_body_type(object, &UF_body_type));

				if (subtype != UF_solid_body_subtype)
					continue;
				if (UF_body_type == UF_MODL_SOLID_BODY)
				{
					// 将对象加入到链表的尾部
					VISUALSAN_NUAA_202_RUN(
						UF_MODL_put_list_item(body_list, object));
				}
			}
			else
			{
				break;
			}

		} while (1);

	}

	// 如果文件存在, 先删除
	remove(ps.c_str());
	// 创建Parasolid文件
	VISUALSAN_NUAA_202_RUN(
		UF_PS_export_data(body_list, (char*)ps.c_str()));
	// 删除链表
	VISUALSAN_NUAA_202_RUN(
		UF_MODL_delete_list(&body_list));

	return 0;
}

