#!/usr/bin/env pwsh

Function smail
{
  Param (
          [String] $TO,
          [String] $FROM,
          [String] $cmd
        )

  $add = $false

  $to_info = @{}
  Get-Content .\mail.config | ForEach-Object
  {
    if($_ -match $TO+":")
    {
      $add = $true
    }
    if($add)
    {
      $to_info.Add($_.Trim().split(":")[0], $_.Trim().split(":")[1])
    }
    if($_.Trim().split(":")[0] -match "port")
    {
      $add = $false
    }
  }

  $from_info = @{}
  Get-Content .\mail.config | ForEach-Object
  {
    if($_ -match $FROM+":")
    {
      $add = $true
    }
    if($add)
    {
      $from_info.Add($_.Trim().split(":")[0], $_.Trim().split(":")[1])
    }
    if($_.Trim().split(":")[0] -match "port")
    {
      $add = $false
    }
  }

  $subject = "Results run $cmd"
  $tmp = -join("Password", $from_info['address'])
  $password = Read-Host -Prompt $tmp -AsSecureString
  #$BSTR = [System.Runtime.InteropServices.Marshal]::SecureStringToBSTR($password)
  #$PlainPassword = [System.Runtime.InteropServices.Marshal]::PtrToStringAuto($BSTR)

  $myLog = "log.txt"
  $stdErrLog = "stderr.log"
  $stdOutLog = "stdout.log"
  Try
  {
    Invoke-Expression $cmd | Out-File $stdOutLog
    $msg = "Success! Script exit with 0. For the output stream see the logfile."
  }
  Catch
  {
    $_ | Out-File $stdErrLog -Append
    $msg = "Somethig goes wrong. Please see the logfile for more informations."
  }
      Get-Content $stdErrLog, $stdOutLog -ErrorAction SilentlyContinue | Out-File $myLog -Encoding ASCII
  Remove-Item $stdOutLog -Force -ErrorAction SilentlyContinue
  Remove-Item $stdErrLog -Force -ErrorAction SilentlyContinue

  $SMTPMessage = New-Object System.Net.Mail.MailMessage($from_info["address"], $to_info["address"], $subject, $msg)
  $attachment = $pwd.Path + "\" + $myLog
  #$SMTPattachment = New-Object System.Net.Mail.Attachment($attachment)
  #$SMTPMessage.Attachments.Add($STMPattachment)

  $SMTPClient = New-Object Net.Mail.SmtpClient($from_info["smtp"], $from_info["port"])
  $SMTPClient.EnableSsl = $true
  $SMTPClient.Credentials = New-Object System.Net.NetworkCredential($from_info["address"].split("@")[0], $password);
  $SMTPClient.Send($SMTPMessage)
  Remove-Variable -Name SMTPClient
  Remove-Variable -Name password

  #$to = $to_info["username"].Trim(), " <" + $to_info["address"].Trim() + ">" -join ""
  #$from = $from_info["username"].Trim(), " <" + $from_info["address"].Trim() + ">" -join ""
  #$smtp = $from_info["smtp"].Trim() -join ""
  #$port = $from_info["port"].Trim() -join ""
  #$send = "Send-MailMessage -From `"$from`" -To `"$to`" -Subject `"ciao`" -Body `"ciao`" -Attachments `"$myLog`" -UseSsl -SmtpServer `"$smtp`" -Port $port"
  #Write-Host $send
  #Invoke-Expression $send
  #Remove-Variable -Name password
}

smail -To "to" -FROM "from" -cmd "dir"
