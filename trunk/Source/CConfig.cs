using System;
using System.Collections.Generic;
using System.Text;
using System.Xml;

namespace server
{
    public class CConfig
    {
        public int Port;
        public string DSN;
        public int CliVersion;

        public void LoadConfigs()
        {
            XmlDocument doc = new XmlDocument();
            doc.Load("./config.xml");
            Port = Convert.ToInt32(doc.GetElementsByTagName("port")[0].InnerText);
            DSN = Convert.ToString(doc.GetElementsByTagName("DSN")[0].InnerText);
            CliVersion = Convert.ToInt32(doc.GetElementsByTagName("ClientVersion")[0].InnerText);
        }
    }
}
