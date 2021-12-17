
namespace FilediskProxyNet
{
    partial class FormRoot
    {
        /// <summary>
        /// Required designer variable.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// Clean up any resources being used.
        /// </summary>
        /// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Windows Form Designer generated code

        /// <summary>
        /// Required method for Designer support - do not modify
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            this.components = new System.ComponentModel.Container();
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(FormRoot));
            this.tabControl1 = new System.Windows.Forms.TabControl();
            this.tabPage1 = new System.Windows.Forms.TabPage();
            this.groupBox3 = new System.Windows.Forms.GroupBox();
            this.cmdClose = new System.Windows.Forms.Button();
            this.label2 = new System.Windows.Forms.Label();
            this.txtDrivePath = new System.Windows.Forms.TextBox();
            this.label17 = new System.Windows.Forms.Label();
            this.txtboxTotalRunningTime = new System.Windows.Forms.TextBox();
            this.chkReadOnlyVHD = new System.Windows.Forms.CheckBox();
            this.txtboxTotalDataWrittenMB = new System.Windows.Forms.TextBox();
            this.txtboxTotalDataReadMB = new System.Windows.Forms.TextBox();
            this.label13 = new System.Windows.Forms.Label();
            this.txtboxTotalDataWritten = new System.Windows.Forms.TextBox();
            this.label11 = new System.Windows.Forms.Label();
            this.cmbDriveLetter = new System.Windows.Forms.ComboBox();
            this.txtboxTotalDataRead = new System.Windows.Forms.TextBox();
            this.label7 = new System.Windows.Forms.Label();
            this.ssVaultFile1 = new System.Windows.Forms.StatusStrip();
            this.tssLabelVaultFile1 = new System.Windows.Forms.ToolStripStatusLabel();
            this.tssLabelBandwidthReadMB = new System.Windows.Forms.ToolStripStatusLabel();
            this.tssLabelBandwidthWriteMB = new System.Windows.Forms.ToolStripStatusLabel();
            this.cmdExploreVault = new System.Windows.Forms.Button();
            this.cmdUnloadFile = new System.Windows.Forms.Button();
            this.label3 = new System.Windows.Forms.Label();
            this.cmdCreateNewFile = new System.Windows.Forms.Button();
            this.txtContainerFile = new System.Windows.Forms.TextBox();
            this.cmdLoadFile = new System.Windows.Forms.Button();
            this.tabPage2 = new System.Windows.Forms.TabPage();
            this.groupBox2 = new System.Windows.Forms.GroupBox();
            this.label4 = new System.Windows.Forms.Label();
            this.radioUseSocket = new System.Windows.Forms.RadioButton();
            this.txtPort = new System.Windows.Forms.TextBox();
            this.radioUsePipe = new System.Windows.Forms.RadioButton();
            this.radioUseSHM = new System.Windows.Forms.RadioButton();
            this.groupBox1 = new System.Windows.Forms.GroupBox();
            this.radioUseSizeGB = new System.Windows.Forms.RadioButton();
            this.label5 = new System.Windows.Forms.Label();
            this.txtConfigureVaultSizeGB = new System.Windows.Forms.TextBox();
            this.radioUseSizeMB = new System.Windows.Forms.RadioButton();
            this.label1 = new System.Windows.Forms.Label();
            this.txtConfigureVaultSizeMB = new System.Windows.Forms.TextBox();
            this.tabPage3 = new System.Windows.Forms.TabPage();
            this.groupBox4 = new System.Windows.Forms.GroupBox();
            this.txtOffsetKB = new System.Windows.Forms.TextBox();
            this.radioOffsetKB = new System.Windows.Forms.RadioButton();
            this.label12 = new System.Windows.Forms.Label();
            this.txtOffsetGB = new System.Windows.Forms.TextBox();
            this.radioOffsetGB = new System.Windows.Forms.RadioButton();
            this.label10 = new System.Windows.Forms.Label();
            this.txtOffsetMB = new System.Windows.Forms.TextBox();
            this.radioOffsetMB = new System.Windows.Forms.RadioButton();
            this.label9 = new System.Windows.Forms.Label();
            this.radioOffsetByte = new System.Windows.Forms.RadioButton();
            this.label6 = new System.Windows.Forms.Label();
            this.txtOffsetByte = new System.Windows.Forms.TextBox();
            this.tabPage4 = new System.Windows.Forms.TabPage();
            this.groupBox5 = new System.Windows.Forms.GroupBox();
            this.txtVirtualImageSizeKB = new System.Windows.Forms.TextBox();
            this.radioVirtualImageSizeKB = new System.Windows.Forms.RadioButton();
            this.label8 = new System.Windows.Forms.Label();
            this.txtVirtualImageSizeGB = new System.Windows.Forms.TextBox();
            this.radioVirtualImageSizeGB = new System.Windows.Forms.RadioButton();
            this.label15 = new System.Windows.Forms.Label();
            this.txtVirtualImageSizeMB = new System.Windows.Forms.TextBox();
            this.radioVirtualImageSizeMB = new System.Windows.Forms.RadioButton();
            this.label16 = new System.Windows.Forms.Label();
            this.radioVirtualImageSizeByte = new System.Windows.Forms.RadioButton();
            this.label18 = new System.Windows.Forms.Label();
            this.txtVirtualImageSizeByte = new System.Windows.Forms.TextBox();
            this.tabPage5 = new System.Windows.Forms.TabPage();
            this.chkLiveVHDLockWriteAccess = new System.Windows.Forms.CheckBox();
            this.label14 = new System.Windows.Forms.Label();
            this.sfdVaultFile = new System.Windows.Forms.SaveFileDialog();
            this.ofdVaultFile = new System.Windows.Forms.OpenFileDialog();
            this.timerUpdateStatus = new System.Windows.Forms.Timer(this.components);
            this.timerCheckContainer1Status = new System.Windows.Forms.Timer(this.components);
            this.tabControl1.SuspendLayout();
            this.tabPage1.SuspendLayout();
            this.groupBox3.SuspendLayout();
            this.ssVaultFile1.SuspendLayout();
            this.tabPage2.SuspendLayout();
            this.groupBox2.SuspendLayout();
            this.groupBox1.SuspendLayout();
            this.tabPage3.SuspendLayout();
            this.groupBox4.SuspendLayout();
            this.tabPage4.SuspendLayout();
            this.groupBox5.SuspendLayout();
            this.tabPage5.SuspendLayout();
            this.SuspendLayout();
            // 
            // tabControl1
            // 
            this.tabControl1.Controls.Add(this.tabPage1);
            this.tabControl1.Controls.Add(this.tabPage2);
            this.tabControl1.Controls.Add(this.tabPage3);
            this.tabControl1.Controls.Add(this.tabPage4);
            this.tabControl1.Controls.Add(this.tabPage5);
            this.tabControl1.Font = new System.Drawing.Font("Microsoft Sans Serif", 14.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.tabControl1.Location = new System.Drawing.Point(12, 12);
            this.tabControl1.Name = "tabControl1";
            this.tabControl1.SelectedIndex = 0;
            this.tabControl1.Size = new System.Drawing.Size(820, 550);
            this.tabControl1.TabIndex = 46;
            // 
            // tabPage1
            // 
            this.tabPage1.Controls.Add(this.groupBox3);
            this.tabPage1.Location = new System.Drawing.Point(4, 33);
            this.tabPage1.Name = "tabPage1";
            this.tabPage1.Padding = new System.Windows.Forms.Padding(3);
            this.tabPage1.Size = new System.Drawing.Size(812, 513);
            this.tabPage1.TabIndex = 0;
            this.tabPage1.Text = "vault container";
            this.tabPage1.UseVisualStyleBackColor = true;
            // 
            // groupBox3
            // 
            this.groupBox3.BackColor = System.Drawing.Color.Ivory;
            this.groupBox3.Controls.Add(this.cmdClose);
            this.groupBox3.Controls.Add(this.label2);
            this.groupBox3.Controls.Add(this.txtDrivePath);
            this.groupBox3.Controls.Add(this.label17);
            this.groupBox3.Controls.Add(this.txtboxTotalRunningTime);
            this.groupBox3.Controls.Add(this.chkReadOnlyVHD);
            this.groupBox3.Controls.Add(this.txtboxTotalDataWrittenMB);
            this.groupBox3.Controls.Add(this.txtboxTotalDataReadMB);
            this.groupBox3.Controls.Add(this.label13);
            this.groupBox3.Controls.Add(this.txtboxTotalDataWritten);
            this.groupBox3.Controls.Add(this.label11);
            this.groupBox3.Controls.Add(this.cmbDriveLetter);
            this.groupBox3.Controls.Add(this.txtboxTotalDataRead);
            this.groupBox3.Controls.Add(this.label7);
            this.groupBox3.Controls.Add(this.ssVaultFile1);
            this.groupBox3.Controls.Add(this.cmdExploreVault);
            this.groupBox3.Controls.Add(this.cmdUnloadFile);
            this.groupBox3.Controls.Add(this.label3);
            this.groupBox3.Controls.Add(this.cmdCreateNewFile);
            this.groupBox3.Controls.Add(this.txtContainerFile);
            this.groupBox3.Controls.Add(this.cmdLoadFile);
            this.groupBox3.Font = new System.Drawing.Font("Microsoft Sans Serif", 11.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.groupBox3.Location = new System.Drawing.Point(6, 7);
            this.groupBox3.Name = "groupBox3";
            this.groupBox3.Size = new System.Drawing.Size(800, 500);
            this.groupBox3.TabIndex = 29;
            this.groupBox3.TabStop = false;
            this.groupBox3.Text = "container 1";
            // 
            // cmdClose
            // 
            this.cmdClose.BackColor = System.Drawing.Color.White;
            this.cmdClose.Font = new System.Drawing.Font("Microsoft Sans Serif", 12F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.cmdClose.Location = new System.Drawing.Point(664, 123);
            this.cmdClose.Name = "cmdClose";
            this.cmdClose.Size = new System.Drawing.Size(130, 30);
            this.cmdClose.TabIndex = 62;
            this.cmdClose.Text = "exit...";
            this.cmdClose.UseVisualStyleBackColor = false;
            this.cmdClose.Click += new System.EventHandler(this.cmdClose_Click);
            // 
            // label2
            // 
            this.label2.AutoSize = true;
            this.label2.Font = new System.Drawing.Font("Microsoft Sans Serif", 11.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.label2.Location = new System.Drawing.Point(5, 326);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(86, 18);
            this.label2.TabIndex = 61;
            this.label2.Text = "drive path:";
            // 
            // txtDrivePath
            // 
            this.txtDrivePath.BackColor = System.Drawing.Color.LightGreen;
            this.txtDrivePath.Font = new System.Drawing.Font("Microsoft Sans Serif", 15.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.txtDrivePath.ForeColor = System.Drawing.Color.Black;
            this.txtDrivePath.Location = new System.Drawing.Point(111, 318);
            this.txtDrivePath.MaxLength = 0;
            this.txtDrivePath.Name = "txtDrivePath";
            this.txtDrivePath.ReadOnly = true;
            this.txtDrivePath.ScrollBars = System.Windows.Forms.ScrollBars.Vertical;
            this.txtDrivePath.Size = new System.Drawing.Size(411, 31);
            this.txtDrivePath.TabIndex = 60;
            this.txtDrivePath.Text = "drive path complete";
            // 
            // label17
            // 
            this.label17.AutoSize = true;
            this.label17.Font = new System.Drawing.Font("Microsoft Sans Serif", 11.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.label17.Location = new System.Drawing.Point(5, 355);
            this.label17.Name = "label17";
            this.label17.Size = new System.Drawing.Size(105, 18);
            this.label17.TabIndex = 59;
            this.label17.Text = "running time:";
            // 
            // txtboxTotalRunningTime
            // 
            this.txtboxTotalRunningTime.BackColor = System.Drawing.Color.WhiteSmoke;
            this.txtboxTotalRunningTime.Font = new System.Drawing.Font("Microsoft Sans Serif", 14.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.txtboxTotalRunningTime.ForeColor = System.Drawing.Color.Black;
            this.txtboxTotalRunningTime.Location = new System.Drawing.Point(111, 355);
            this.txtboxTotalRunningTime.MaxLength = 0;
            this.txtboxTotalRunningTime.Multiline = true;
            this.txtboxTotalRunningTime.Name = "txtboxTotalRunningTime";
            this.txtboxTotalRunningTime.ReadOnly = true;
            this.txtboxTotalRunningTime.ScrollBars = System.Windows.Forms.ScrollBars.Horizontal;
            this.txtboxTotalRunningTime.Size = new System.Drawing.Size(547, 52);
            this.txtboxTotalRunningTime.TabIndex = 58;
            this.txtboxTotalRunningTime.WordWrap = false;
            // 
            // chkReadOnlyVHD
            // 
            this.chkReadOnlyVHD.AutoSize = true;
            this.chkReadOnlyVHD.Font = new System.Drawing.Font("Microsoft Sans Serif", 9.75F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.chkReadOnlyVHD.Location = new System.Drawing.Point(220, 105);
            this.chkReadOnlyVHD.Name = "chkReadOnlyVHD";
            this.chkReadOnlyVHD.Size = new System.Drawing.Size(306, 20);
            this.chkReadOnlyVHD.TabIndex = 31;
            this.chkReadOnlyVHD.Text = "load as read-only VHD (no write access)";
            this.chkReadOnlyVHD.UseVisualStyleBackColor = true;
            // 
            // txtboxTotalDataWrittenMB
            // 
            this.txtboxTotalDataWrittenMB.BackColor = System.Drawing.Color.DimGray;
            this.txtboxTotalDataWrittenMB.Font = new System.Drawing.Font("Microsoft Sans Serif", 9.75F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.txtboxTotalDataWrittenMB.ForeColor = System.Drawing.Color.WhiteSmoke;
            this.txtboxTotalDataWrittenMB.Location = new System.Drawing.Point(414, 450);
            this.txtboxTotalDataWrittenMB.MaxLength = 0;
            this.txtboxTotalDataWrittenMB.Name = "txtboxTotalDataWrittenMB";
            this.txtboxTotalDataWrittenMB.ReadOnly = true;
            this.txtboxTotalDataWrittenMB.ScrollBars = System.Windows.Forms.ScrollBars.Vertical;
            this.txtboxTotalDataWrittenMB.Size = new System.Drawing.Size(380, 22);
            this.txtboxTotalDataWrittenMB.TabIndex = 54;
            // 
            // txtboxTotalDataReadMB
            // 
            this.txtboxTotalDataReadMB.BackColor = System.Drawing.Color.DimGray;
            this.txtboxTotalDataReadMB.Font = new System.Drawing.Font("Microsoft Sans Serif", 9.75F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.txtboxTotalDataReadMB.ForeColor = System.Drawing.Color.WhiteSmoke;
            this.txtboxTotalDataReadMB.Location = new System.Drawing.Point(10, 450);
            this.txtboxTotalDataReadMB.MaxLength = 0;
            this.txtboxTotalDataReadMB.Name = "txtboxTotalDataReadMB";
            this.txtboxTotalDataReadMB.ReadOnly = true;
            this.txtboxTotalDataReadMB.ScrollBars = System.Windows.Forms.ScrollBars.Vertical;
            this.txtboxTotalDataReadMB.Size = new System.Drawing.Size(380, 22);
            this.txtboxTotalDataReadMB.TabIndex = 53;
            // 
            // label13
            // 
            this.label13.AutoSize = true;
            this.label13.Font = new System.Drawing.Font("Microsoft Sans Serif", 11.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.label13.Location = new System.Drawing.Point(407, 406);
            this.label13.Name = "label13";
            this.label13.Size = new System.Drawing.Size(138, 18);
            this.label13.TabIndex = 26;
            this.label13.Text = "total data written:";
            // 
            // txtboxTotalDataWritten
            // 
            this.txtboxTotalDataWritten.BackColor = System.Drawing.Color.DimGray;
            this.txtboxTotalDataWritten.Font = new System.Drawing.Font("Microsoft Sans Serif", 9.75F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.txtboxTotalDataWritten.ForeColor = System.Drawing.Color.WhiteSmoke;
            this.txtboxTotalDataWritten.Location = new System.Drawing.Point(414, 427);
            this.txtboxTotalDataWritten.MaxLength = 0;
            this.txtboxTotalDataWritten.Name = "txtboxTotalDataWritten";
            this.txtboxTotalDataWritten.ReadOnly = true;
            this.txtboxTotalDataWritten.ScrollBars = System.Windows.Forms.ScrollBars.Vertical;
            this.txtboxTotalDataWritten.Size = new System.Drawing.Size(380, 22);
            this.txtboxTotalDataWritten.TabIndex = 25;
            // 
            // label11
            // 
            this.label11.AutoSize = true;
            this.label11.Font = new System.Drawing.Font("Microsoft Sans Serif", 12F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.label11.Location = new System.Drawing.Point(6, 78);
            this.label11.Name = "label11";
            this.label11.Size = new System.Drawing.Size(86, 20);
            this.label11.TabIndex = 49;
            this.label11.Text = "drive letter:";
            // 
            // cmbDriveLetter
            // 
            this.cmbDriveLetter.DisplayMember = "1";
            this.cmbDriveLetter.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.cmbDriveLetter.Font = new System.Drawing.Font("Microsoft Sans Serif", 14.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.cmbDriveLetter.FormattingEnabled = true;
            this.cmbDriveLetter.Items.AddRange(new object[] {
            "A",
            "B",
            "C",
            "D",
            "E",
            "F",
            "G",
            "H",
            "I",
            "J",
            "K",
            "L",
            "M",
            "N",
            "O",
            "P",
            "Q",
            "R",
            "S",
            "T",
            "U",
            "V",
            "W",
            "X",
            "Y",
            "Z"});
            this.cmbDriveLetter.Location = new System.Drawing.Point(220, 72);
            this.cmbDriveLetter.Name = "cmbDriveLetter";
            this.cmbDriveLetter.Size = new System.Drawing.Size(438, 32);
            this.cmbDriveLetter.TabIndex = 48;
            this.cmbDriveLetter.ValueMember = "1";
            // 
            // txtboxTotalDataRead
            // 
            this.txtboxTotalDataRead.BackColor = System.Drawing.Color.DimGray;
            this.txtboxTotalDataRead.Font = new System.Drawing.Font("Microsoft Sans Serif", 9.75F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.txtboxTotalDataRead.ForeColor = System.Drawing.Color.WhiteSmoke;
            this.txtboxTotalDataRead.Location = new System.Drawing.Point(10, 427);
            this.txtboxTotalDataRead.MaxLength = 0;
            this.txtboxTotalDataRead.Name = "txtboxTotalDataRead";
            this.txtboxTotalDataRead.ReadOnly = true;
            this.txtboxTotalDataRead.ScrollBars = System.Windows.Forms.ScrollBars.Vertical;
            this.txtboxTotalDataRead.Size = new System.Drawing.Size(380, 22);
            this.txtboxTotalDataRead.TabIndex = 23;
            // 
            // label7
            // 
            this.label7.AutoSize = true;
            this.label7.Font = new System.Drawing.Font("Microsoft Sans Serif", 11.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.label7.Location = new System.Drawing.Point(4, 406);
            this.label7.Name = "label7";
            this.label7.Size = new System.Drawing.Size(121, 18);
            this.label7.TabIndex = 24;
            this.label7.Text = "total data read:";
            // 
            // ssVaultFile1
            // 
            this.ssVaultFile1.Font = new System.Drawing.Font("Verdana", 6.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.ssVaultFile1.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.tssLabelVaultFile1,
            this.tssLabelBandwidthReadMB,
            this.tssLabelBandwidthWriteMB});
            this.ssVaultFile1.Location = new System.Drawing.Point(3, 475);
            this.ssVaultFile1.Name = "ssVaultFile1";
            this.ssVaultFile1.Size = new System.Drawing.Size(794, 22);
            this.ssVaultFile1.SizingGrip = false;
            this.ssVaultFile1.TabIndex = 29;
            // 
            // tssLabelVaultFile1
            // 
            this.tssLabelVaultFile1.AutoSize = false;
            this.tssLabelVaultFile1.Name = "tssLabelVaultFile1";
            this.tssLabelVaultFile1.Size = new System.Drawing.Size(280, 17);
            this.tssLabelVaultFile1.Text = "no file loaded.";
            this.tssLabelVaultFile1.TextAlign = System.Drawing.ContentAlignment.MiddleLeft;
            // 
            // tssLabelBandwidthReadMB
            // 
            this.tssLabelBandwidthReadMB.AutoSize = false;
            this.tssLabelBandwidthReadMB.Name = "tssLabelBandwidthReadMB";
            this.tssLabelBandwidthReadMB.Size = new System.Drawing.Size(245, 17);
            this.tssLabelBandwidthReadMB.Text = "read: MB/s";
            this.tssLabelBandwidthReadMB.TextAlign = System.Drawing.ContentAlignment.MiddleLeft;
            // 
            // tssLabelBandwidthWriteMB
            // 
            this.tssLabelBandwidthWriteMB.AutoSize = false;
            this.tssLabelBandwidthWriteMB.Name = "tssLabelBandwidthWriteMB";
            this.tssLabelBandwidthWriteMB.Size = new System.Drawing.Size(245, 17);
            this.tssLabelBandwidthWriteMB.Text = "write: MB/s";
            this.tssLabelBandwidthWriteMB.TextAlign = System.Drawing.ContentAlignment.MiddleLeft;
            // 
            // cmdExploreVault
            // 
            this.cmdExploreVault.BackColor = System.Drawing.Color.Gold;
            this.cmdExploreVault.Font = new System.Drawing.Font("Microsoft Sans Serif", 12F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.cmdExploreVault.Location = new System.Drawing.Point(528, 318);
            this.cmdExploreVault.Name = "cmdExploreVault";
            this.cmdExploreVault.Size = new System.Drawing.Size(130, 30);
            this.cmdExploreVault.TabIndex = 28;
            this.cmdExploreVault.Text = "explore...";
            this.cmdExploreVault.UseVisualStyleBackColor = false;
            this.cmdExploreVault.Click += new System.EventHandler(this.cmdExploreVault_Click);
            // 
            // cmdUnloadFile
            // 
            this.cmdUnloadFile.BackColor = System.Drawing.Color.LightCoral;
            this.cmdUnloadFile.Font = new System.Drawing.Font("Microsoft Sans Serif", 12F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.cmdUnloadFile.Location = new System.Drawing.Point(664, 87);
            this.cmdUnloadFile.Name = "cmdUnloadFile";
            this.cmdUnloadFile.Size = new System.Drawing.Size(130, 30);
            this.cmdUnloadFile.TabIndex = 26;
            this.cmdUnloadFile.Text = "unload file...";
            this.cmdUnloadFile.UseVisualStyleBackColor = false;
            this.cmdUnloadFile.Click += new System.EventHandler(this.cmdUnloadFile_Click);
            // 
            // label3
            // 
            this.label3.AutoSize = true;
            this.label3.Font = new System.Drawing.Font("Microsoft Sans Serif", 11.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.label3.Location = new System.Drawing.Point(3, 104);
            this.label3.Name = "label3";
            this.label3.Size = new System.Drawing.Size(165, 18);
            this.label3.TabIndex = 22;
            this.label3.Text = "loaded container file:";
            // 
            // cmdCreateNewFile
            // 
            this.cmdCreateNewFile.BackColor = System.Drawing.Color.LightYellow;
            this.cmdCreateNewFile.Font = new System.Drawing.Font("Microsoft Sans Serif", 12F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.cmdCreateNewFile.Location = new System.Drawing.Point(664, 15);
            this.cmdCreateNewFile.Name = "cmdCreateNewFile";
            this.cmdCreateNewFile.Size = new System.Drawing.Size(130, 30);
            this.cmdCreateNewFile.TabIndex = 16;
            this.cmdCreateNewFile.Text = "new file...";
            this.cmdCreateNewFile.UseVisualStyleBackColor = false;
            this.cmdCreateNewFile.Click += new System.EventHandler(this.cmdCreateNewFile_Click);
            // 
            // txtContainerFile
            // 
            this.txtContainerFile.BackColor = System.Drawing.Color.DimGray;
            this.txtContainerFile.Font = new System.Drawing.Font("Microsoft Sans Serif", 15.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.txtContainerFile.ForeColor = System.Drawing.Color.WhiteSmoke;
            this.txtContainerFile.Location = new System.Drawing.Point(9, 125);
            this.txtContainerFile.MaxLength = 0;
            this.txtContainerFile.Multiline = true;
            this.txtContainerFile.Name = "txtContainerFile";
            this.txtContainerFile.ReadOnly = true;
            this.txtContainerFile.ScrollBars = System.Windows.Forms.ScrollBars.Vertical;
            this.txtContainerFile.Size = new System.Drawing.Size(649, 73);
            this.txtContainerFile.TabIndex = 20;
            // 
            // cmdLoadFile
            // 
            this.cmdLoadFile.BackColor = System.Drawing.Color.Yellow;
            this.cmdLoadFile.Font = new System.Drawing.Font("Microsoft Sans Serif", 12F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.cmdLoadFile.Location = new System.Drawing.Point(664, 51);
            this.cmdLoadFile.Name = "cmdLoadFile";
            this.cmdLoadFile.Size = new System.Drawing.Size(130, 30);
            this.cmdLoadFile.TabIndex = 23;
            this.cmdLoadFile.Text = "load file...";
            this.cmdLoadFile.UseVisualStyleBackColor = false;
            this.cmdLoadFile.Click += new System.EventHandler(this.cmdLoadFile_Click);
            // 
            // tabPage2
            // 
            this.tabPage2.BackColor = System.Drawing.Color.Cornsilk;
            this.tabPage2.Controls.Add(this.groupBox2);
            this.tabPage2.Controls.Add(this.groupBox1);
            this.tabPage2.Location = new System.Drawing.Point(4, 33);
            this.tabPage2.Name = "tabPage2";
            this.tabPage2.Size = new System.Drawing.Size(812, 513);
            this.tabPage2.TabIndex = 1;
            this.tabPage2.Text = "configuration";
            // 
            // groupBox2
            // 
            this.groupBox2.Controls.Add(this.label4);
            this.groupBox2.Controls.Add(this.radioUseSocket);
            this.groupBox2.Controls.Add(this.txtPort);
            this.groupBox2.Controls.Add(this.radioUsePipe);
            this.groupBox2.Controls.Add(this.radioUseSHM);
            this.groupBox2.Location = new System.Drawing.Point(3, 175);
            this.groupBox2.Name = "groupBox2";
            this.groupBox2.Size = new System.Drawing.Size(795, 180);
            this.groupBox2.TabIndex = 55;
            this.groupBox2.TabStop = false;
            this.groupBox2.Text = "configure request i/o type";
            // 
            // label4
            // 
            this.label4.AutoSize = true;
            this.label4.Font = new System.Drawing.Font("Microsoft Sans Serif", 15.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.label4.Location = new System.Drawing.Point(19, 94);
            this.label4.Name = "label4";
            this.label4.Size = new System.Drawing.Size(133, 25);
            this.label4.TabIndex = 43;
            this.label4.Text = "port number:";
            // 
            // radioUseSocket
            // 
            this.radioUseSocket.AutoSize = true;
            this.radioUseSocket.Location = new System.Drawing.Point(24, 122);
            this.radioUseSocket.Name = "radioUseSocket";
            this.radioUseSocket.Size = new System.Drawing.Size(206, 28);
            this.radioUseSocket.TabIndex = 42;
            this.radioUseSocket.Text = "use windows sockets";
            this.radioUseSocket.UseVisualStyleBackColor = true;
            this.radioUseSocket.CheckedChanged += new System.EventHandler(this.radioUseSocket_CheckedChanged);
            // 
            // txtPort
            // 
            this.txtPort.Enabled = false;
            this.txtPort.Font = new System.Drawing.Font("Microsoft Sans Serif", 24F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.txtPort.ForeColor = System.Drawing.Color.Black;
            this.txtPort.Location = new System.Drawing.Point(274, 98);
            this.txtPort.MaxLength = 0;
            this.txtPort.Name = "txtPort";
            this.txtPort.Size = new System.Drawing.Size(368, 44);
            this.txtPort.TabIndex = 42;
            this.txtPort.Text = "1";
            // 
            // radioUsePipe
            // 
            this.radioUsePipe.AutoSize = true;
            this.radioUsePipe.Location = new System.Drawing.Point(24, 62);
            this.radioUsePipe.Name = "radioUsePipe";
            this.radioUsePipe.Size = new System.Drawing.Size(101, 28);
            this.radioUsePipe.TabIndex = 41;
            this.radioUsePipe.Text = "use pipe";
            this.radioUsePipe.UseVisualStyleBackColor = true;
            // 
            // radioUseSHM
            // 
            this.radioUseSHM.AutoSize = true;
            this.radioUseSHM.Checked = true;
            this.radioUseSHM.Location = new System.Drawing.Point(24, 28);
            this.radioUseSHM.Name = "radioUseSHM";
            this.radioUseSHM.Size = new System.Drawing.Size(196, 28);
            this.radioUseSHM.TabIndex = 38;
            this.radioUseSHM.TabStop = true;
            this.radioUseSHM.Text = "use shared memory";
            this.radioUseSHM.UseVisualStyleBackColor = true;
            // 
            // groupBox1
            // 
            this.groupBox1.Controls.Add(this.radioUseSizeGB);
            this.groupBox1.Controls.Add(this.label5);
            this.groupBox1.Controls.Add(this.txtConfigureVaultSizeGB);
            this.groupBox1.Controls.Add(this.radioUseSizeMB);
            this.groupBox1.Controls.Add(this.label1);
            this.groupBox1.Controls.Add(this.txtConfigureVaultSizeMB);
            this.groupBox1.Location = new System.Drawing.Point(3, 15);
            this.groupBox1.Name = "groupBox1";
            this.groupBox1.Size = new System.Drawing.Size(795, 156);
            this.groupBox1.TabIndex = 54;
            this.groupBox1.TabStop = false;
            this.groupBox1.Text = "configure size";
            // 
            // radioUseSizeGB
            // 
            this.radioUseSizeGB.AutoSize = true;
            this.radioUseSizeGB.Location = new System.Drawing.Point(24, 120);
            this.radioUseSizeGB.Name = "radioUseSizeGB";
            this.radioUseSizeGB.Size = new System.Drawing.Size(149, 28);
            this.radioUseSizeGB.TabIndex = 41;
            this.radioUseSizeGB.Text = "use this option";
            this.radioUseSizeGB.UseVisualStyleBackColor = true;
            // 
            // label5
            // 
            this.label5.AutoSize = true;
            this.label5.Font = new System.Drawing.Font("Microsoft Sans Serif", 15.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.label5.Location = new System.Drawing.Point(19, 92);
            this.label5.Name = "label5";
            this.label5.Size = new System.Drawing.Size(199, 25);
            this.label5.TabIndex = 40;
            this.label5.Text = "size (in giga bytes):";
            // 
            // txtConfigureVaultSizeGB
            // 
            this.txtConfigureVaultSizeGB.Font = new System.Drawing.Font("Microsoft Sans Serif", 24F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.txtConfigureVaultSizeGB.ForeColor = System.Drawing.Color.Black;
            this.txtConfigureVaultSizeGB.Location = new System.Drawing.Point(274, 96);
            this.txtConfigureVaultSizeGB.MaxLength = 0;
            this.txtConfigureVaultSizeGB.Name = "txtConfigureVaultSizeGB";
            this.txtConfigureVaultSizeGB.Size = new System.Drawing.Size(368, 44);
            this.txtConfigureVaultSizeGB.TabIndex = 39;
            this.txtConfigureVaultSizeGB.Text = "1";
            // 
            // radioUseSizeMB
            // 
            this.radioUseSizeMB.AutoSize = true;
            this.radioUseSizeMB.Checked = true;
            this.radioUseSizeMB.Location = new System.Drawing.Point(24, 61);
            this.radioUseSizeMB.Name = "radioUseSizeMB";
            this.radioUseSizeMB.Size = new System.Drawing.Size(149, 28);
            this.radioUseSizeMB.TabIndex = 38;
            this.radioUseSizeMB.TabStop = true;
            this.radioUseSizeMB.Text = "use this option";
            this.radioUseSizeMB.UseVisualStyleBackColor = true;
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Font = new System.Drawing.Font("Microsoft Sans Serif", 15.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.label1.Location = new System.Drawing.Point(19, 33);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(211, 25);
            this.label1.TabIndex = 37;
            this.label1.Text = "size (in mega bytes):";
            // 
            // txtConfigureVaultSizeMB
            // 
            this.txtConfigureVaultSizeMB.Font = new System.Drawing.Font("Microsoft Sans Serif", 24F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.txtConfigureVaultSizeMB.ForeColor = System.Drawing.Color.Black;
            this.txtConfigureVaultSizeMB.Location = new System.Drawing.Point(274, 37);
            this.txtConfigureVaultSizeMB.MaxLength = 0;
            this.txtConfigureVaultSizeMB.Name = "txtConfigureVaultSizeMB";
            this.txtConfigureVaultSizeMB.Size = new System.Drawing.Size(368, 44);
            this.txtConfigureVaultSizeMB.TabIndex = 36;
            this.txtConfigureVaultSizeMB.Text = "100";
            this.txtConfigureVaultSizeMB.TextChanged += new System.EventHandler(this.txtConfigureVaultSizeMB_TextChanged);
            // 
            // tabPage3
            // 
            this.tabPage3.BackColor = System.Drawing.Color.Ivory;
            this.tabPage3.Controls.Add(this.groupBox4);
            this.tabPage3.Location = new System.Drawing.Point(4, 33);
            this.tabPage3.Name = "tabPage3";
            this.tabPage3.Size = new System.Drawing.Size(812, 513);
            this.tabPage3.TabIndex = 4;
            this.tabPage3.Text = "image offset";
            // 
            // groupBox4
            // 
            this.groupBox4.Controls.Add(this.txtOffsetKB);
            this.groupBox4.Controls.Add(this.radioOffsetKB);
            this.groupBox4.Controls.Add(this.label12);
            this.groupBox4.Controls.Add(this.txtOffsetGB);
            this.groupBox4.Controls.Add(this.radioOffsetGB);
            this.groupBox4.Controls.Add(this.label10);
            this.groupBox4.Controls.Add(this.txtOffsetMB);
            this.groupBox4.Controls.Add(this.radioOffsetMB);
            this.groupBox4.Controls.Add(this.label9);
            this.groupBox4.Controls.Add(this.radioOffsetByte);
            this.groupBox4.Controls.Add(this.label6);
            this.groupBox4.Controls.Add(this.txtOffsetByte);
            this.groupBox4.Location = new System.Drawing.Point(3, 3);
            this.groupBox4.Name = "groupBox4";
            this.groupBox4.Size = new System.Drawing.Size(795, 286);
            this.groupBox4.TabIndex = 57;
            this.groupBox4.TabStop = false;
            this.groupBox4.Text = "configure virtual image offset";
            // 
            // txtOffsetKB
            // 
            this.txtOffsetKB.Font = new System.Drawing.Font("Microsoft Sans Serif", 24F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.txtOffsetKB.ForeColor = System.Drawing.Color.Black;
            this.txtOffsetKB.Location = new System.Drawing.Point(273, 104);
            this.txtOffsetKB.MaxLength = 0;
            this.txtOffsetKB.Name = "txtOffsetKB";
            this.txtOffsetKB.Size = new System.Drawing.Size(516, 44);
            this.txtOffsetKB.TabIndex = 56;
            this.txtOffsetKB.Text = "0";
            // 
            // radioOffsetKB
            // 
            this.radioOffsetKB.AutoSize = true;
            this.radioOffsetKB.Location = new System.Drawing.Point(30, 115);
            this.radioOffsetKB.Name = "radioOffsetKB";
            this.radioOffsetKB.Size = new System.Drawing.Size(149, 28);
            this.radioOffsetKB.TabIndex = 55;
            this.radioOffsetKB.Text = "use this option";
            this.radioOffsetKB.UseVisualStyleBackColor = true;
            // 
            // label12
            // 
            this.label12.AutoSize = true;
            this.label12.Font = new System.Drawing.Font("Microsoft Sans Serif", 15.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.label12.Location = new System.Drawing.Point(25, 87);
            this.label12.Name = "label12";
            this.label12.Size = new System.Drawing.Size(171, 25);
            this.label12.TabIndex = 54;
            this.label12.Text = "offset (kilo byte):";
            // 
            // txtOffsetGB
            // 
            this.txtOffsetGB.Font = new System.Drawing.Font("Microsoft Sans Serif", 24F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.txtOffsetGB.ForeColor = System.Drawing.Color.Black;
            this.txtOffsetGB.Location = new System.Drawing.Point(273, 222);
            this.txtOffsetGB.MaxLength = 0;
            this.txtOffsetGB.Name = "txtOffsetGB";
            this.txtOffsetGB.Size = new System.Drawing.Size(516, 44);
            this.txtOffsetGB.TabIndex = 53;
            this.txtOffsetGB.Text = "0";
            // 
            // radioOffsetGB
            // 
            this.radioOffsetGB.AutoSize = true;
            this.radioOffsetGB.Location = new System.Drawing.Point(30, 233);
            this.radioOffsetGB.Name = "radioOffsetGB";
            this.radioOffsetGB.Size = new System.Drawing.Size(149, 28);
            this.radioOffsetGB.TabIndex = 52;
            this.radioOffsetGB.Text = "use this option";
            this.radioOffsetGB.UseVisualStyleBackColor = true;
            // 
            // label10
            // 
            this.label10.AutoSize = true;
            this.label10.Font = new System.Drawing.Font("Microsoft Sans Serif", 15.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.label10.Location = new System.Drawing.Point(25, 205);
            this.label10.Name = "label10";
            this.label10.Size = new System.Drawing.Size(179, 25);
            this.label10.TabIndex = 51;
            this.label10.Text = "offset (giga byte):";
            // 
            // txtOffsetMB
            // 
            this.txtOffsetMB.Font = new System.Drawing.Font("Microsoft Sans Serif", 24F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.txtOffsetMB.ForeColor = System.Drawing.Color.Black;
            this.txtOffsetMB.Location = new System.Drawing.Point(273, 163);
            this.txtOffsetMB.MaxLength = 0;
            this.txtOffsetMB.Name = "txtOffsetMB";
            this.txtOffsetMB.Size = new System.Drawing.Size(516, 44);
            this.txtOffsetMB.TabIndex = 50;
            this.txtOffsetMB.Text = "0";
            // 
            // radioOffsetMB
            // 
            this.radioOffsetMB.AutoSize = true;
            this.radioOffsetMB.Location = new System.Drawing.Point(30, 174);
            this.radioOffsetMB.Name = "radioOffsetMB";
            this.radioOffsetMB.Size = new System.Drawing.Size(149, 28);
            this.radioOffsetMB.TabIndex = 49;
            this.radioOffsetMB.Text = "use this option";
            this.radioOffsetMB.UseVisualStyleBackColor = true;
            // 
            // label9
            // 
            this.label9.AutoSize = true;
            this.label9.Font = new System.Drawing.Font("Microsoft Sans Serif", 15.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.label9.Location = new System.Drawing.Point(25, 146);
            this.label9.Name = "label9";
            this.label9.Size = new System.Drawing.Size(191, 25);
            this.label9.TabIndex = 48;
            this.label9.Text = "offset (mega byte):";
            // 
            // radioOffsetByte
            // 
            this.radioOffsetByte.AutoSize = true;
            this.radioOffsetByte.Checked = true;
            this.radioOffsetByte.Location = new System.Drawing.Point(30, 56);
            this.radioOffsetByte.Name = "radioOffsetByte";
            this.radioOffsetByte.Size = new System.Drawing.Size(149, 28);
            this.radioOffsetByte.TabIndex = 46;
            this.radioOffsetByte.TabStop = true;
            this.radioOffsetByte.Text = "use this option";
            this.radioOffsetByte.UseVisualStyleBackColor = true;
            // 
            // label6
            // 
            this.label6.AutoSize = true;
            this.label6.Font = new System.Drawing.Font("Microsoft Sans Serif", 15.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.label6.Location = new System.Drawing.Point(25, 28);
            this.label6.Name = "label6";
            this.label6.Size = new System.Drawing.Size(132, 25);
            this.label6.TabIndex = 43;
            this.label6.Text = "offset (byte):";
            // 
            // txtOffsetByte
            // 
            this.txtOffsetByte.Font = new System.Drawing.Font("Microsoft Sans Serif", 24F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.txtOffsetByte.ForeColor = System.Drawing.Color.Black;
            this.txtOffsetByte.Location = new System.Drawing.Point(273, 45);
            this.txtOffsetByte.MaxLength = 0;
            this.txtOffsetByte.Name = "txtOffsetByte";
            this.txtOffsetByte.Size = new System.Drawing.Size(516, 44);
            this.txtOffsetByte.TabIndex = 42;
            this.txtOffsetByte.Text = "0";
            // 
            // tabPage4
            // 
            this.tabPage4.BackColor = System.Drawing.Color.LightYellow;
            this.tabPage4.Controls.Add(this.groupBox5);
            this.tabPage4.Location = new System.Drawing.Point(4, 33);
            this.tabPage4.Name = "tabPage4";
            this.tabPage4.Size = new System.Drawing.Size(812, 513);
            this.tabPage4.TabIndex = 5;
            this.tabPage4.Text = "image size";
            // 
            // groupBox5
            // 
            this.groupBox5.Controls.Add(this.txtVirtualImageSizeKB);
            this.groupBox5.Controls.Add(this.radioVirtualImageSizeKB);
            this.groupBox5.Controls.Add(this.label8);
            this.groupBox5.Controls.Add(this.txtVirtualImageSizeGB);
            this.groupBox5.Controls.Add(this.radioVirtualImageSizeGB);
            this.groupBox5.Controls.Add(this.label15);
            this.groupBox5.Controls.Add(this.txtVirtualImageSizeMB);
            this.groupBox5.Controls.Add(this.radioVirtualImageSizeMB);
            this.groupBox5.Controls.Add(this.label16);
            this.groupBox5.Controls.Add(this.radioVirtualImageSizeByte);
            this.groupBox5.Controls.Add(this.label18);
            this.groupBox5.Controls.Add(this.txtVirtualImageSizeByte);
            this.groupBox5.Location = new System.Drawing.Point(3, 3);
            this.groupBox5.Name = "groupBox5";
            this.groupBox5.Size = new System.Drawing.Size(795, 286);
            this.groupBox5.TabIndex = 58;
            this.groupBox5.TabStop = false;
            this.groupBox5.Text = "configure virtual image size";
            // 
            // txtVirtualImageSizeKB
            // 
            this.txtVirtualImageSizeKB.Font = new System.Drawing.Font("Microsoft Sans Serif", 24F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.txtVirtualImageSizeKB.ForeColor = System.Drawing.Color.Black;
            this.txtVirtualImageSizeKB.Location = new System.Drawing.Point(273, 104);
            this.txtVirtualImageSizeKB.MaxLength = 0;
            this.txtVirtualImageSizeKB.Name = "txtVirtualImageSizeKB";
            this.txtVirtualImageSizeKB.Size = new System.Drawing.Size(516, 44);
            this.txtVirtualImageSizeKB.TabIndex = 56;
            this.txtVirtualImageSizeKB.Text = "0";
            // 
            // radioVirtualImageSizeKB
            // 
            this.radioVirtualImageSizeKB.AutoSize = true;
            this.radioVirtualImageSizeKB.Location = new System.Drawing.Point(30, 115);
            this.radioVirtualImageSizeKB.Name = "radioVirtualImageSizeKB";
            this.radioVirtualImageSizeKB.Size = new System.Drawing.Size(149, 28);
            this.radioVirtualImageSizeKB.TabIndex = 55;
            this.radioVirtualImageSizeKB.Text = "use this option";
            this.radioVirtualImageSizeKB.UseVisualStyleBackColor = true;
            // 
            // label8
            // 
            this.label8.AutoSize = true;
            this.label8.Font = new System.Drawing.Font("Microsoft Sans Serif", 15.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.label8.Location = new System.Drawing.Point(25, 87);
            this.label8.Name = "label8";
            this.label8.Size = new System.Drawing.Size(157, 25);
            this.label8.TabIndex = 54;
            this.label8.Text = "size (kilo byte):";
            // 
            // txtVirtualImageSizeGB
            // 
            this.txtVirtualImageSizeGB.Font = new System.Drawing.Font("Microsoft Sans Serif", 24F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.txtVirtualImageSizeGB.ForeColor = System.Drawing.Color.Black;
            this.txtVirtualImageSizeGB.Location = new System.Drawing.Point(273, 222);
            this.txtVirtualImageSizeGB.MaxLength = 0;
            this.txtVirtualImageSizeGB.Name = "txtVirtualImageSizeGB";
            this.txtVirtualImageSizeGB.Size = new System.Drawing.Size(516, 44);
            this.txtVirtualImageSizeGB.TabIndex = 53;
            this.txtVirtualImageSizeGB.Text = "0";
            // 
            // radioVirtualImageSizeGB
            // 
            this.radioVirtualImageSizeGB.AutoSize = true;
            this.radioVirtualImageSizeGB.Location = new System.Drawing.Point(30, 233);
            this.radioVirtualImageSizeGB.Name = "radioVirtualImageSizeGB";
            this.radioVirtualImageSizeGB.Size = new System.Drawing.Size(149, 28);
            this.radioVirtualImageSizeGB.TabIndex = 52;
            this.radioVirtualImageSizeGB.Text = "use this option";
            this.radioVirtualImageSizeGB.UseVisualStyleBackColor = true;
            // 
            // label15
            // 
            this.label15.AutoSize = true;
            this.label15.Font = new System.Drawing.Font("Microsoft Sans Serif", 15.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.label15.Location = new System.Drawing.Point(25, 205);
            this.label15.Name = "label15";
            this.label15.Size = new System.Drawing.Size(165, 25);
            this.label15.TabIndex = 51;
            this.label15.Text = "size (giga byte):";
            // 
            // txtVirtualImageSizeMB
            // 
            this.txtVirtualImageSizeMB.Font = new System.Drawing.Font("Microsoft Sans Serif", 24F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.txtVirtualImageSizeMB.ForeColor = System.Drawing.Color.Black;
            this.txtVirtualImageSizeMB.Location = new System.Drawing.Point(273, 163);
            this.txtVirtualImageSizeMB.MaxLength = 0;
            this.txtVirtualImageSizeMB.Name = "txtVirtualImageSizeMB";
            this.txtVirtualImageSizeMB.Size = new System.Drawing.Size(516, 44);
            this.txtVirtualImageSizeMB.TabIndex = 50;
            this.txtVirtualImageSizeMB.Text = "0";
            // 
            // radioVirtualImageSizeMB
            // 
            this.radioVirtualImageSizeMB.AutoSize = true;
            this.radioVirtualImageSizeMB.Location = new System.Drawing.Point(30, 174);
            this.radioVirtualImageSizeMB.Name = "radioVirtualImageSizeMB";
            this.radioVirtualImageSizeMB.Size = new System.Drawing.Size(149, 28);
            this.radioVirtualImageSizeMB.TabIndex = 49;
            this.radioVirtualImageSizeMB.Text = "use this option";
            this.radioVirtualImageSizeMB.UseVisualStyleBackColor = true;
            // 
            // label16
            // 
            this.label16.AutoSize = true;
            this.label16.Font = new System.Drawing.Font("Microsoft Sans Serif", 15.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.label16.Location = new System.Drawing.Point(25, 146);
            this.label16.Name = "label16";
            this.label16.Size = new System.Drawing.Size(177, 25);
            this.label16.TabIndex = 48;
            this.label16.Text = "size (mega byte):";
            // 
            // radioVirtualImageSizeByte
            // 
            this.radioVirtualImageSizeByte.AutoSize = true;
            this.radioVirtualImageSizeByte.Checked = true;
            this.radioVirtualImageSizeByte.Location = new System.Drawing.Point(30, 56);
            this.radioVirtualImageSizeByte.Name = "radioVirtualImageSizeByte";
            this.radioVirtualImageSizeByte.Size = new System.Drawing.Size(149, 28);
            this.radioVirtualImageSizeByte.TabIndex = 46;
            this.radioVirtualImageSizeByte.TabStop = true;
            this.radioVirtualImageSizeByte.Text = "use this option";
            this.radioVirtualImageSizeByte.UseVisualStyleBackColor = true;
            // 
            // label18
            // 
            this.label18.AutoSize = true;
            this.label18.Font = new System.Drawing.Font("Microsoft Sans Serif", 15.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.label18.Location = new System.Drawing.Point(25, 28);
            this.label18.Name = "label18";
            this.label18.Size = new System.Drawing.Size(118, 25);
            this.label18.TabIndex = 43;
            this.label18.Text = "size (byte):";
            // 
            // txtVirtualImageSizeByte
            // 
            this.txtVirtualImageSizeByte.Font = new System.Drawing.Font("Microsoft Sans Serif", 24F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.txtVirtualImageSizeByte.ForeColor = System.Drawing.Color.Black;
            this.txtVirtualImageSizeByte.Location = new System.Drawing.Point(273, 45);
            this.txtVirtualImageSizeByte.MaxLength = 0;
            this.txtVirtualImageSizeByte.Name = "txtVirtualImageSizeByte";
            this.txtVirtualImageSizeByte.Size = new System.Drawing.Size(516, 44);
            this.txtVirtualImageSizeByte.TabIndex = 42;
            this.txtVirtualImageSizeByte.Text = "0";
            // 
            // tabPage5
            // 
            this.tabPage5.BackColor = System.Drawing.Color.Khaki;
            this.tabPage5.Controls.Add(this.chkLiveVHDLockWriteAccess);
            this.tabPage5.Controls.Add(this.label14);
            this.tabPage5.Location = new System.Drawing.Point(4, 33);
            this.tabPage5.Name = "tabPage5";
            this.tabPage5.Size = new System.Drawing.Size(812, 513);
            this.tabPage5.TabIndex = 3;
            this.tabPage5.Text = "live config";
            // 
            // chkLiveVHDLockWriteAccess
            // 
            this.chkLiveVHDLockWriteAccess.AutoSize = true;
            this.chkLiveVHDLockWriteAccess.Font = new System.Drawing.Font("Microsoft Sans Serif", 18F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.chkLiveVHDLockWriteAccess.Location = new System.Drawing.Point(248, 20);
            this.chkLiveVHDLockWriteAccess.Name = "chkLiveVHDLockWriteAccess";
            this.chkLiveVHDLockWriteAccess.Size = new System.Drawing.Size(469, 33);
            this.chkLiveVHDLockWriteAccess.TabIndex = 33;
            this.chkLiveVHDLockWriteAccess.Text = "check to lock write-access to the VHD";
            this.chkLiveVHDLockWriteAccess.UseVisualStyleBackColor = true;
            // 
            // label14
            // 
            this.label14.AutoSize = true;
            this.label14.Font = new System.Drawing.Font("Microsoft Sans Serif", 11.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.label14.Location = new System.Drawing.Point(3, 26);
            this.label14.Name = "label14";
            this.label14.Size = new System.Drawing.Size(216, 18);
            this.label14.TabIndex = 32;
            this.label14.Text = "live write-access lockdown:";
            // 
            // sfdVaultFile
            // 
            this.sfdVaultFile.DefaultExt = "Vault";
            this.sfdVaultFile.Filter = "Vault files *.Vault|*.Vault|All files *.*|*.*";
            this.sfdVaultFile.Title = "Create a new Vault Container file";
            // 
            // ofdVaultFile
            // 
            this.ofdVaultFile.DefaultExt = "Vault";
            this.ofdVaultFile.Filter = "Vault files *.Vault|*.Vault|All files *.*|*.*";
            this.ofdVaultFile.Title = "Load a Vault Container file";
            // 
            // timerUpdateStatus
            // 
            this.timerUpdateStatus.Interval = 1000;
            this.timerUpdateStatus.Tick += new System.EventHandler(this.timerUpdateStatus_Tick);
            // 
            // timerCheckContainer1Status
            // 
            this.timerCheckContainer1Status.Tick += new System.EventHandler(this.timerCheckContainer1Status_Tick);
            // 
            // FormRoot
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.BackgroundImage = global::FilediskProxyNet.Properties.Resources._1a4f7f15ace98f6aa610193e98e9e16d;
            this.ClientSize = new System.Drawing.Size(884, 611);
            this.Controls.Add(this.tabControl1);
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedDialog;
            this.Icon = ((System.Drawing.Icon)(resources.GetObject("$this.Icon")));
            this.MaximizeBox = false;
            this.MinimizeBox = false;
            this.Name = "FormRoot";
            this.StartPosition = System.Windows.Forms.FormStartPosition.CenterScreen;
            this.Text = "Filedisk Proxy .NET (work in progress)";
            this.FormClosing += new System.Windows.Forms.FormClosingEventHandler(this.FrmRoot_FormClosing);
            this.Load += new System.EventHandler(this.FormRoot_Load);
            this.tabControl1.ResumeLayout(false);
            this.tabPage1.ResumeLayout(false);
            this.groupBox3.ResumeLayout(false);
            this.groupBox3.PerformLayout();
            this.ssVaultFile1.ResumeLayout(false);
            this.ssVaultFile1.PerformLayout();
            this.tabPage2.ResumeLayout(false);
            this.groupBox2.ResumeLayout(false);
            this.groupBox2.PerformLayout();
            this.groupBox1.ResumeLayout(false);
            this.groupBox1.PerformLayout();
            this.tabPage3.ResumeLayout(false);
            this.groupBox4.ResumeLayout(false);
            this.groupBox4.PerformLayout();
            this.tabPage4.ResumeLayout(false);
            this.groupBox5.ResumeLayout(false);
            this.groupBox5.PerformLayout();
            this.tabPage5.ResumeLayout(false);
            this.tabPage5.PerformLayout();
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.TabControl tabControl1;
        private System.Windows.Forms.TabPage tabPage1;
        private System.Windows.Forms.GroupBox groupBox3;
        private System.Windows.Forms.Label label17;
        private System.Windows.Forms.TextBox txtboxTotalRunningTime;
        private System.Windows.Forms.CheckBox chkReadOnlyVHD;
        private System.Windows.Forms.TextBox txtboxTotalDataWrittenMB;
        private System.Windows.Forms.TextBox txtboxTotalDataReadMB;
        private System.Windows.Forms.Label label13;
        private System.Windows.Forms.TextBox txtboxTotalDataWritten;
        private System.Windows.Forms.Label label11;
        private System.Windows.Forms.ComboBox cmbDriveLetter;
        private System.Windows.Forms.TextBox txtboxTotalDataRead;
        private System.Windows.Forms.Label label7;
        private System.Windows.Forms.StatusStrip ssVaultFile1;
        private System.Windows.Forms.ToolStripStatusLabel tssLabelVaultFile1;
        private System.Windows.Forms.ToolStripStatusLabel tssLabelBandwidthReadMB;
        private System.Windows.Forms.ToolStripStatusLabel tssLabelBandwidthWriteMB;
        private System.Windows.Forms.Button cmdExploreVault;
        private System.Windows.Forms.Button cmdUnloadFile;
        private System.Windows.Forms.Label label3;
        private System.Windows.Forms.Button cmdCreateNewFile;
        private System.Windows.Forms.TextBox txtContainerFile;
        private System.Windows.Forms.Button cmdLoadFile;
        private System.Windows.Forms.TabPage tabPage2;
        private System.Windows.Forms.GroupBox groupBox1;
        private System.Windows.Forms.RadioButton radioUseSizeGB;
        private System.Windows.Forms.Label label5;
        private System.Windows.Forms.TextBox txtConfigureVaultSizeGB;
        private System.Windows.Forms.RadioButton radioUseSizeMB;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.TextBox txtConfigureVaultSizeMB;
        private System.Windows.Forms.TabPage tabPage5;
        private System.Windows.Forms.CheckBox chkLiveVHDLockWriteAccess;
        private System.Windows.Forms.Label label14;
        private System.Windows.Forms.SaveFileDialog sfdVaultFile;
        private System.Windows.Forms.OpenFileDialog ofdVaultFile;
        private System.Windows.Forms.Timer timerUpdateStatus;
        private System.Windows.Forms.Timer timerCheckContainer1Status;
        private System.Windows.Forms.Label label2;
        private System.Windows.Forms.TextBox txtDrivePath;
        private System.Windows.Forms.GroupBox groupBox2;
        private System.Windows.Forms.RadioButton radioUsePipe;
        private System.Windows.Forms.RadioButton radioUseSHM;
        private System.Windows.Forms.RadioButton radioUseSocket;
        private System.Windows.Forms.Label label4;
        private System.Windows.Forms.TextBox txtPort;
        private System.Windows.Forms.Button cmdClose;
        private System.Windows.Forms.TabPage tabPage3;
        private System.Windows.Forms.GroupBox groupBox4;
        private System.Windows.Forms.TextBox txtOffsetMB;
        private System.Windows.Forms.RadioButton radioOffsetMB;
        private System.Windows.Forms.Label label9;
        private System.Windows.Forms.RadioButton radioOffsetByte;
        private System.Windows.Forms.Label label6;
        private System.Windows.Forms.TextBox txtOffsetByte;
        private System.Windows.Forms.TextBox txtOffsetGB;
        private System.Windows.Forms.RadioButton radioOffsetGB;
        private System.Windows.Forms.Label label10;
        private System.Windows.Forms.TextBox txtOffsetKB;
        private System.Windows.Forms.RadioButton radioOffsetKB;
        private System.Windows.Forms.Label label12;
        private System.Windows.Forms.TabPage tabPage4;
        private System.Windows.Forms.GroupBox groupBox5;
        private System.Windows.Forms.TextBox txtVirtualImageSizeKB;
        private System.Windows.Forms.RadioButton radioVirtualImageSizeKB;
        private System.Windows.Forms.Label label8;
        private System.Windows.Forms.TextBox txtVirtualImageSizeGB;
        private System.Windows.Forms.RadioButton radioVirtualImageSizeGB;
        private System.Windows.Forms.Label label15;
        private System.Windows.Forms.TextBox txtVirtualImageSizeMB;
        private System.Windows.Forms.RadioButton radioVirtualImageSizeMB;
        private System.Windows.Forms.Label label16;
        private System.Windows.Forms.RadioButton radioVirtualImageSizeByte;
        private System.Windows.Forms.Label label18;
        private System.Windows.Forms.TextBox txtVirtualImageSizeByte;
    }
}

