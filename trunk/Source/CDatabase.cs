using System;
using System.Collections.Generic;
using System.Text;
using System.Data;
using System.Windows.Forms;
using MySql.Data;
using MySql.Data.MySqlClient;

namespace server
{
    public class CDatabase
    {
        private MySqlConnection db;
        public MySqlDataReader result;
        public MySqlDataReader extresult;
        private MySqlCommand cmd;
        private MySqlCommand cmdExt;

        public CDatabase(string host, string user, string pwd, string database, int port)
        {
            string connStr = "Database=" + database + ";Server=" + host + ";Port=" + port + ";User Id=" + user + ";Password=" + pwd;

            db = new MySqlConnection(connStr);
            try
            {
                if (db.State == ConnectionState.Closed)
                {
                    db.Open();
                    Core.CLog("Connected on MySQL Database Server");
                    Core.CLog(String.Format("[MySQL Version: {0}]", db.ServerVersion));
                }
            }
            catch(MySqlException Ex)
            {
                Core.CLog(Ex.Message);
                MessageBox.Show("Could not access the database.\r\nPlease make sure you have configured settings correctly.\r\n\r\nMore details:\r\n" + Ex.Message, "Database connection error", MessageBoxButtons.OK, MessageBoxIcon.Error);
            }
        }

        public void QStore(string query)
        {
            if (db.State == ConnectionState.Open)
            {
                cmd = new MySqlCommand(query, db);
                result = cmd.ExecuteReader();
            }
        }

        public void QStoreExt(string query)
        {
            if (db.State == ConnectionState.Open)
            {
                cmdExt = new MySqlCommand(query, db);
                extresult = cmdExt.ExecuteReader();
            }
        }

        public void Query(string query)
        {
            if (db.State == ConnectionState.Open)
            {
                cmd = new MySqlCommand(query, db);
            }
        }

        public void FreeQuery()
        {
            if (db.State == ConnectionState.Open)
            {
                result.Close();
            }
        }

        public void FreeQueryExt()
        {
            if (db.State == ConnectionState.Open)
            {
                extresult.Close();
            }
        }
    }
}
