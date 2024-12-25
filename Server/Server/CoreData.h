#pragma once
//====�������������=========
#include <qstring.h>
#include <qbytearray.h>

#include <QString>
#include <QUuid>
#include <QDateTime>
#include <QFile>
#include <QDebug>
#include <QFileInfo>
namespace btyGoose
{
	/// <summary>
	/// ����������ݽṹ
	/// </summary>
	namespace data
	{
		struct Account	
		{
			///�˻�����
			enum Type
			{
				CONSUMER,	//�������˻�
				MERCHANT,	//�̼��˻�
				ADMIN		//����Ա�˻�
			};
			enum Level
			{
				MEMBER,		//��ͨ�û�
				VIP,		//VIP�û�
				SUVIP		//����VIP�û�
			};

			QString uuid = "";		//Ψһid
			QString name = "";		//�˻���
			QString nickname = "";	//�ǳ�
			QByteArray icon = QByteArray();	//ͼ��
			Type type = CONSUMER;			//�˻�����
			double balance = 0;		//�˻����
			QString phone = "";		//�绰����
			Level level = MEMBER;		//�Żݵȼ�
		};

		class Dish
		{
			QString uuid = "";			//��Ʒid
			QString merchant_id = "";	//�̼�uuid
			QString name = "";			//��Ʒ����
			QString icon_path = "";		//����ͼƬ��url
			QString description = "";	//��Ʒ������
			double base_price = 0;		//�����۸�
			double price_factor = 1;	//�۸�Ӱ������
		};

		struct Order
		{
			enum Status
			{
				UNPAYED,		//δ֧��״̬
				WAITING,		//�ȴ��̼Ҵ���
				OVER_TIME,		//������ʱ
				REJECTED,		//�̼Ҿ���
				SUCCESS,		//�ɹ����
				ERR,			//��������
				FATAL			//����������������
			};

			QString merchant_id;		//�̼�id
			QString merchant_name;		//�̼��˻�����
			QString consumer_id;		//������id
			QString consumer_name;		//�������˻�����
			QString dish_id;			//��Ʒid
			QString dish_name;			//��Ʒ����
			QString base_price;			//�����۸�
			QString price_factor;		//�۸�Ӱ������
			QString time;				//����ʱ��
			Account::Level level;		//�Żݵȼ�
			double pay;					//�����۸�
			QString uuid;				//����id
			Status status;				//����״̬
		};
	}
}