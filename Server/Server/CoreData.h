#pragma once
#include <qstring.h>
#include <qbytearray.h>

#include <string>
#include <QString>
#include <QUuid>
#include <QDateTime>
#include <QFile>
#include <QDebug>
#include <QFileInfo>
#include <iostream>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonValue>
#include <QFile>
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
			QString password = "";	//����
			QString nickname = "";	//�ǳ�
			QByteArray icon = QByteArray();	//ͼ��
			Type type = CONSUMER;			//�˻�����
			double balance = 0;		//�˻����
			QString phone = "";		//�绰����
			Level level = MEMBER;		//�Żݵȼ�

			bool loadFromJson(const std::string& jsonStr)
			{
				QString qJsonString = QString::fromStdString(jsonStr);

				// ���� JSON �ַ���
				QJsonDocument doc = QJsonDocument::fromJson(qJsonString.toUtf8());
				if (doc.isObject()) {
					QJsonObject jsonObj = doc.object();

					// �� JSON �м������ݵ��ṹ���Ա
					if (jsonObj.contains("uuid") && jsonObj["uuid"].isString()) {
						uuid = jsonObj["uuid"].toString();
					}

					if (jsonObj.contains("name") && jsonObj["name"].isString()) {
						name = jsonObj["name"].toString();
					}

					if (jsonObj.contains("password") && jsonObj["password"].isString()) {
						name = jsonObj["password"].toString();
					}

					if (jsonObj.contains("nickname") && jsonObj["nickname"].isString()) {
						nickname = jsonObj["nickname"].toString();
					}

					if (jsonObj.contains("icon") && jsonObj["icon"].isString()) {
						icon = QByteArray::fromBase64(jsonObj["icon"].toString().toUtf8());
					}

					if (jsonObj.contains("type") && jsonObj["type"].isString()) {
						QString typeStr = jsonObj["type"].toString();
						if (typeStr == "CONSUMER") {
							type = CONSUMER;
						}
						else if (typeStr == "MERCHANT") {
							type = MERCHANT;
						}
						else if (typeStr == "ADMIN") {
							type = ADMIN;
						}
					}

					if (jsonObj.contains("balance") && jsonObj["balance"].isDouble()) {
						balance = jsonObj["balance"].toDouble();
					}

					if (jsonObj.contains("phone") && jsonObj["phone"].isString()) {
						phone = jsonObj["phone"].toString();
					}

					if (jsonObj.contains("level") && jsonObj["level"].isString()) {
						QString levelStr = jsonObj["level"].toString();
						if (levelStr == "MEMBER") {
							level = MEMBER;
						}
						else if (levelStr == "VIP") {
							level = VIP;
						}
						else if (levelStr == "SUVIP") {
							level = SUVIP;
						}
					}


					return true;
				}
				else
				{
					qDebug() << "Invalid Json string : " << jsonStr;
					return false;
				}
			}
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

			bool loadFromJson(const std::string& jsonStr)
			{
				QString qJsonString = QString::fromStdString(jsonStr);

				// ���� JSON �ַ���
				QJsonDocument doc = QJsonDocument::fromJson(qJsonString.toUtf8());
				if (doc.isObject()) {
					QJsonObject jsonObj = doc.object();

					// �� JSON �м������ݵ���Ա����
					if (jsonObj.contains("uuid") && jsonObj["uuid"].isString()) {
						uuid = jsonObj["uuid"].toString();
					}

					if (jsonObj.contains("merchant_id") && jsonObj["merchant_id"].isString()) {
						merchant_id = jsonObj["merchant_id"].toString();
					}

					if (jsonObj.contains("name") && jsonObj["name"].isString()) {
						name = jsonObj["name"].toString();
					}

					if (jsonObj.contains("icon_path") && jsonObj["icon_path"].isString()) {
						icon_path = jsonObj["icon_path"].toString();
					}

					if (jsonObj.contains("description") && jsonObj["description"].isString()) {
						description = jsonObj["description"].toString();
					}

					if (jsonObj.contains("base_price") && jsonObj["base_price"].isDouble()) {
						base_price = jsonObj["base_price"].toDouble();
					}

					if (jsonObj.contains("price_factor") && jsonObj["price_factor"].isDouble()) {
						price_factor = jsonObj["price_factor"].toDouble();
					}

					return true;

				}
				else
				{
					qDebug() << "Invalid Json string : " << jsonStr;
					return false;
				}
			}
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


			bool loadFromJson(const std::string& jsonStr)
			{
				QString qJsonString = QString::fromStdString(jsonStr);

				// ���� JSON �ַ���
				QJsonDocument doc = QJsonDocument::fromJson(qJsonString.toUtf8());
				if (doc.isObject()) {
					QJsonObject jsonObj = doc.object();

					// �� JSON �м������ݵ���Ա����
					if (jsonObj.contains("merchant_id") && jsonObj["merchant_id"].isString()) {
						merchant_id = jsonObj["merchant_id"].toString();
					}

					if (jsonObj.contains("merchant_name") && jsonObj["merchant_name"].isString()) {
						merchant_name = jsonObj["merchant_name"].toString();
					}

					if (jsonObj.contains("consumer_id") && jsonObj["consumer_id"].isString()) {
						consumer_id = jsonObj["consumer_id"].toString();
					}

					if (jsonObj.contains("consumer_name") && jsonObj["consumer_name"].isString()) {
						consumer_name = jsonObj["consumer_name"].toString();
					}

					if (jsonObj.contains("dish_id") && jsonObj["dish_id"].isString()) {
						dish_id = jsonObj["dish_id"].toString();
					}

					if (jsonObj.contains("dish_name") && jsonObj["dish_name"].isString()) {
						dish_name = jsonObj["dish_name"].toString();
					}

					if (jsonObj.contains("base_price") && jsonObj["base_price"].isString()) {
						base_price = jsonObj["base_price"].toString();
					}

					if (jsonObj.contains("price_factor") && jsonObj["price_factor"].isString()) {
						price_factor = jsonObj["price_factor"].toString();
					}

					if (jsonObj.contains("time") && jsonObj["time"].isString()) {
						time = jsonObj["time"].toString();
					}

					if (jsonObj.contains("level") && jsonObj["level"].isString()) {
						QString levelStr = jsonObj["level"].toString();
						if (levelStr == "MEMBER") {
							level = Account::MEMBER;
						}
						else if (levelStr == "VIP") {
							level = Account::VIP;
						}
						else if (levelStr == "SUVIP") {
							level = Account::SUVIP;
						}
					}

					if (jsonObj.contains("pay") && jsonObj["pay"].isDouble()) {
						pay = jsonObj["pay"].toDouble();
					}

					if (jsonObj.contains("uuid") && jsonObj["uuid"].isString()) {
						uuid = jsonObj["uuid"].toString();
					}

					if (jsonObj.contains("status") && jsonObj["status"].isString()) {
						QString statusStr = jsonObj["status"].toString();
						if (statusStr == "UNPAYED") {
							status = UNPAYED;
						}
						else if (statusStr == "WAITING") {
							status = WAITING;
						}
						else if (statusStr == "OVER_TIME") {
							status = OVER_TIME;
						}
						else if (statusStr == "REJECTED") {
							status = REJECTED;
						}
						else if (statusStr == "SUCCESS") {
							status = SUCCESS;
						}
						else if (statusStr == "ERR") {
							status = ERR;
						}
						else if (statusStr == "FATAL") {
							status = FATAL;
						}
					}

					return true;

				}
				else
				{
					qDebug() << "Invalid Json string : " << jsonStr;
					return false;
				}
			}
		};
	};
}