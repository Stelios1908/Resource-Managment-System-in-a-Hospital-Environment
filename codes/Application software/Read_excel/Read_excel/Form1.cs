using ExcelDataReader;
using Microsoft.VisualBasic.ApplicationServices;
using System.IO;
using System.Text;

namespace Read_excel
{
    public partial class Form1 : Form
    {
        public Form1()
        {
            InitializeComponent();
        }

        private void Form1_Load(object sender, EventArgs e)
        {

        }

        private void button1_Click(object sender, EventArgs e)
        {

            OpenFileDialog openFileDialog = new OpenFileDialog();
            if (openFileDialog.ShowDialog() == DialogResult.OK)
            {
                textBox1.Text = openFileDialog.FileName;
            }
        }

        private void button2_Click(object sender, EventArgs e)
        {
            //string path = "C:\\Users\\30698\\Desktop\\sideris\\PTIXIAKI\\CODE\\SMART_HOSPITAL\\log.xlsx";
            System.Text.Encoding.RegisterProvider(System.Text.CodePagesEncodingProvider.Instance);

         

            using (var stream = File.Open(textBox1.Text, FileMode.Open, FileAccess.Read))
            {
                using (var reader = ExcelReaderFactory.CreateReader(stream))
                {
                    var data = reader.AsDataSet();
                    string[] newColumnNames = { "DATE-TIME", "NAME", "QUALITY", "ROOM","ACTION" };

                    for (int i = 0; i < data.Tables[0].Columns.Count; i++)
                    {
                        data.Tables[0].Columns[i].ColumnName = newColumnNames[i]; 
                    }


                    if (data.Tables.Count > 0 && data.Tables[0].Rows.Count > 0)
                    {
                        data.Tables[0].Rows.RemoveAt(0);
                    }

                    dataGridView1.DataSource = data.Tables[0];

                }
            }

        }
    }
}