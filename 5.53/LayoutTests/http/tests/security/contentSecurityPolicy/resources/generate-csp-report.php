<meta http-equiv="Content-Security-Policy" content="script-src 'self'; report-uri save-report.php?test=<?php echo $_GET['test']; ?>">
<script>
// This script block will trigger a violation report.
alert('FAIL');
</script>
<script src="go-to-echo-report.php?test=<?php echo $_GET['test']; ?>"></script>