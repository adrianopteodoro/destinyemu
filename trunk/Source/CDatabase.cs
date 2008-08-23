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
        bool m_Init;

        public CDatabase(string connStr)
        {
            db = new OdbcConnection(connStr);
            try
            {
                if (db.State == ConnectionState.Closed)
                {
                    db.Open();
                    Core.CLog("Odbc connected on database server");
                    //Core.CLog(String.Format("[Odbc Version: {0}]", db.ServerVersion));
                    m_Init = true;
                }
            }
            catch(OdbcException Ex)
            {
                Core.CLog(Ex.Message);
                MessageBox.Show("Could not access the database.\r\nPlease make sure you have configured settings correctly.\r\n\r\nMore details:\r\n" + Ex.Message, "Database connection error", MessageBoxButtons.OK, MessageBoxIcon.Error);
                m_Init = false;
            }
        }

        public bool GetStatus()
        {
            return m_Init;
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
