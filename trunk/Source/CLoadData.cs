using System;
using System.Collections.Generic;
using System.Text;
using System.Xml;

namespace server
{
    class CLoadData
    {
        public static void LoadItemList(CServer server)
        {
            server.NumLoadedItems = 0;
            XmlDocument doc = new XmlDocument();
            try
            {
                doc.Load("./Data/ItemList.xml");
            }
            catch (Exception ex)
            {
                Core.CLog(String.Format("Error: {0}", ex.Message));
            }
            XmlNode root = doc.DocumentElement;
            XmlNodeList list = root.SelectNodes("/itemlist/item");
            for (int i = 0; i < list.Count; i++)
            {
                server.ItemList[i].Effects = new CEffect[11];
                server.ItemList[i].ItemID = int.Parse(list.Item(i).Attributes.Item(0).Value);
                server.ItemList[i].Name = list.Item(i).Attributes.Item(1).Value;
                server.ItemList[i].Mesh = int.Parse(list.Item(i).Attributes.Item(2).Value);
                server.ItemList[i].Type = int.Parse(list.Item(i).Attributes.Item(3).Value);
                server.ItemList[i].ReqLvl = int.Parse(list.Item(i).Attributes.Item(4).Value);
                server.ItemList[i].ReqStr = int.Parse(list.Item(i).Attributes.Item(5).Value);
                server.ItemList[i].ReqInt = int.Parse(list.Item(i).Attributes.Item(6).Value);
                server.ItemList[i].ReqDex = int.Parse(list.Item(i).Attributes.Item(7).Value);
                server.ItemList[i].ReqCon = int.Parse(list.Item(i).Attributes.Item(8).Value);
                server.ItemList[i].Price = int.Parse(list.Item(i).Attributes.Item(9).Value);
                server.ItemList[i].Pos = int.Parse(list.Item(i).Attributes.Item(10).Value);
                server.ItemList[i].Extreme = int.Parse(list.Item(i).Attributes.Item(11).Value);
                server.ItemList[i].Grade = int.Parse(list.Item(i).Attributes.Item(12).Value);
                for (int x = 0; x < 11; x++)
                {
                    server.ItemList[i].Effects[x].Effect = int.Parse(list.Item(i).Attributes.Item((2 * x) + 13).Value);
                    server.ItemList[i].Effects[x].Value = int.Parse(list.Item(i).Attributes.Item((2 * x) + 14).Value);
                }
                server.NumLoadedItems++;
            }
            Core.CLog(String.Format("Loaded {0} Items from Database.", server.NumLoadedItems));
        }
    }
}
