<set author="Emanuele Paolini">
<set email="paolini@sns.it">
<if sourcefilelist="">Nessun file
<else>
  <foreach file $sourcefilelist>
  <message "Processing file $file">
  <process>
    <set outputfile=<stat base file=$file>.html>
    <set sourcefile=$file>
    <clear><file input=$file><file output><nospace>
  </process>
  </foreach>
</if>
<clear><nospace>
