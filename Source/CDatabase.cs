using System;
using System.Collections.Generic;
using System.Text;
using System.Data;
using System.Data.Odbc;
using System.Windows.Forms;

namespace server
{
    public class CDatabase
    {
        private OdbcConnection db;
        public OdbcDataReader result;
        public OdbcDataReader extresult;
        private OdbcCommand cmd;
        private OdbcCommand cmdExt;

        public CDatabase(string host, string user, string pwd, string database, int port)
        {
            string connStr = "DRIVER={MySQL ODBC 3.51 Driver};SERVER=" + host + ";PORT=" + port + ";UID=" + user + ";PWD=" + pwd + ";DATABASE=" + database + ";OPTION=3;";
            db = new OdbcConnection(connStr);
            try
            {
                if (db.State == ConnectionState.Closed)
                {
                    db.Open();
                    Core.CLog("Connected on MySQL Database Server");
                    Core.CLog(String.Format("[MySQL Version: {0}]", db.ServerVersion));
                }
            }
            catch(OdbcException Ex)
            {
                Core.CLog(Ex.Message);
                MessageBox.Show("Could not access the database.\r\nPlease make sure you have configured DSN Setting Correctly.\r\n\r\nMore details:\r\n" + Ex.Message, "Database connection error", MessageBoxButtons.OK, MessageBoxIcon.Error);
            }
        }

        public void QStore(string query)
        {
            if (db.State == ConnectionState.Open)
            {
                cmd = new OdbcCommand(query, db);
                result = cmd.ExecuteReader();
            }
        }

        public void QStoreExt(string query)
        {
            if (db.State == ConnectionState.Open)
            {
                cmdExt = new OdbcCommand(query, db);
                extresult = cmdExt.ExecuteReader();
            }
        }

        public void Query(string query)
        {
            if (db.State == ConnectionState.Open)
            {
                cmd = new OdbcCommand(query, db);
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
