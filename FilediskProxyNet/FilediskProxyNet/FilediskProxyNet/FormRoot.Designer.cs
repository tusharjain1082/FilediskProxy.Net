
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
            this.SuspendLayout();
            // 
            // tabControl1
            // 
            this.tabControl1.Controls.Add(this.tabPage1);
            this.tabControl1.Controls.Add(this.tabPage2);
            this.tabControl1.Controls.Add(this.tabPage3);
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
            this.groupBox2.Controls.Add(this.radioUsePipe);
            this.groupBox2.Controls.Add(this.radioUseSHM);
            this.groupBox2.Location = new System.Drawing.Point(3, 214);
            this.groupBox2.Name = "groupBox2";
            this.groupBox2.Size = new System.Drawing.Size(795, 100);
            this.groupBox2.TabIndex = 55;
            this.groupBox2.TabStop = false;
            this.groupBox2.Text = "configure request i/o type";
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
            this.groupBox1.Size = new System.Drawing.Size(795, 193);
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
            // 
            // tabPage3
            // 
            this.tabPage3.BackColor = System.Drawing.Color.Khaki;
            this.tabPage3.Controls.Add(this.chkLiveVHDLockWriteAccess);
            this.tabPage3.Controls.Add(this.label14);
            this.tabPage3.Location = new System.Drawing.Point(4, 33);
            this.tabPage3.Name = "tabPage3";
            this.tabPage3.Size = new System.Drawing.Size(812, 513);
            this.tabPage3.TabIndex = 3;
            this.tabPage3.Text = "live config";
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
            this.tabPage3.PerformLayout();
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
        private System.Windows.Forms.TabPage tabPage3;
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
    }
}

