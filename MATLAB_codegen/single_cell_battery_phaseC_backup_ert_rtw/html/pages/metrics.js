function CodeMetrics() {
	 this.metricsArray = {};
	 this.metricsArray.var = new Array();
	 this.metricsArray.fcn = new Array();
	 this.metricsArray.fcn["look2_iflf_binlc"] = {file: "C:\\Users\\mahad\\OneDrive\\Documents\\MATLAB\\slprj\\ert\\_sharedutils\\look2_iflf_binlc.c",
	stack: 88,
	stackTotal: 88};
	 this.metricsArray.fcn["look2_iflf_pbinlc"] = {file: "C:\\Users\\mahad\\OneDrive\\Documents\\MATLAB\\slprj\\ert\\_sharedutils\\look2_iflf_pbinlc.c",
	stack: 100,
	stackTotal: 100};
	 this.metricsArray.fcn["single_cell_battery_phaseC_backup_initialize"] = {file: "C:\\Users\\mahad\\OneDrive\\Documents\\MATLAB\\single_cell_battery_phaseC_backup_ert_rtw\\single_cell_battery_phaseC_backup.c",
	stack: 16,
	stackTotal: 16};
	 this.metricsArray.fcn["single_cell_battery_phaseC_backup_step"] = {file: "C:\\Users\\mahad\\OneDrive\\Documents\\MATLAB\\single_cell_battery_phaseC_backup_ert_rtw\\single_cell_battery_phaseC_backup.c",
	stack: 84,
	stackTotal: 184};
	 this.metricsArray.fcn["single_cell_battery_phaseC_backup_terminate"] = {file: "C:\\Users\\mahad\\OneDrive\\Documents\\MATLAB\\single_cell_battery_phaseC_backup_ert_rtw\\single_cell_battery_phaseC_backup.c",
	stack: 8,
	stackTotal: 8};
	 this.getMetrics = function(token) { 
		 var data;
		 data = this.metricsArray.var[token];
		 if (!data) {
			 data = this.metricsArray.fcn[token];
			 if (data) data.type = "fcn";
		 } else { 
			 data.type = "var";
		 }
	 return data; }; 
	 this.codeMetricsSummary = '<a href="javascript:void(0)" onclick="return postParentWindowMessage({message:\'gotoReportPage\', pageName:\'single_cell_battery_phaseC_backup_metrics\'});">Global Memory: 0(bytes) Maximum Stack: 100(bytes)</a>';
	}
CodeMetrics.instance = new CodeMetrics();
