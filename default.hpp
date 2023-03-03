hpp style sheet
manu-fatto
<message "  default file sheet">
<def it><if $lang=it></def><def /it></if></def>
<def en><if $lang=en></def><def /en></if></def>
<if not author><set author="Emanuele Paolini"></if>
<if not email><set email="<img src='../email.png' align='center' alt='paolini math unifi it' />"></if>
<if not home><set home="http://web.math.unifi.it/users/paolini/"></if>
<if not up><set up="../"></if>
<if not help><set help="&home;help/"></if>
<set e="\&egrave;">
<set a="\&agrave;">
<set i="\&igrave;">
<set o="\&ograve;">
<set u="\&ugrave;">
<set amp="\&">
<set PS="<img src='&home;ps.png' alt='PS' border='0'>">
<set PDF="<img src='&home;pdf.png' alt='PDF' border='0'>">
<set PNG="<img src='&home;png.png' alt='PNG' border='0'>">
<set buttons><nospace>
<div align=right bgcolor="#0000ff">
<if home><nospace>
<if not "&home;"=""><nospace>
<a href="&home;"><nospace>
</if><img src="&home;home.png" alt=Home border=0><nospace>
<if not "&home;"=""></a></if></if><nospace>

<if up><if not "&up;"=""><a href="&up;"><img src="&home;up.png" alt=Up border=0></a><nospace></if>
</if>

<if left><a href="&left;"><img src="&home;left.png" alt=<it>Precedente</it><en>Previous</en> border=0></a></if><nospace>

<if right><a href="&right;"><img src="&home;right.png" alt=<it>Successivo</it><en>Next</en> border=0></a></if><nospace>

<if help><a href="&help;"><img src="&home;help.png" alt="<it>Aiuto!</it><en>Help</en>" border=0></a></if><nospace>

<foreach lang2 &languages;><nospace>
<set flag><img src="&home;&lang2;.png" alt="&lang2;"; border=0></set>
  <if not &lang2;=&lang;>
    <a href="&baseoutputfile;.&lang2;.html"><use flag></a>
  <else>
    <use flag>
  </if>
</foreach>
</div>
</set>

<def body>
<if not languages><set languages="it"></if>
<if not title><set title="senza titolo"></if>
<if not header><set header="Emanuele Paolini: &title;"></if>
<foreach lang $languages>
<process>
<message "  processing language &lang;" >
<set baseoutputfile=<stat base file="&outputfile;">>
<set outputfile="&baseoutputfile;.&lang;.html">
<clear><nospace>
<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.0//&lang;">
<html>
<head>
<meta http-equiv="Content-Type" content="text/html; charset=iso-8859-1">
<link rev="made" title="&author;" href="mailto:&email;">
<title><use header></title>
</head>
<literal><body</literal> lang="&lang;" <if bgcolor>bgcolor=&bgcolor;<else>bgcolor="#60C0Ff"</if>>
<use buttons><nospace>
<hr>
<if not "&title;"=""><h1 align=center><use title></h1></if>
</def>
<def /body>
<hr>
<use buttons>
<emph>
<en>Last updated: <stat date format="%a %b %d %Y"></en>
<it>Ultima modifica: <stat date format="%d %m %Y"></it>
<ignore>
-
<a href="mailto:&email;">&email;</a></ignore>
<literal></body></literal><nospace>
<message "    writing to output file $outputfile"><nospace>
<file output="&outputfile;"><nospace>
</process>
</foreach>
<clear>
</def><nospace>
<def date>
<set mese01=gennaio>
<set mese02=febbraio>
<set mese03=marzo>
<set mese04=aprile>
<set mese05=maggio>
<set mese06=giugno>
<set mese07=luglio>
<set mese08=agosto>
<set mese09=settembre>
<set mese10=ottobre>
<set mese11=novembre>
<set mese12=dicembre>
<set ref>https://hermes.math.unifi.it/mrbs/day.php?year=&year;&amp;month=&month;&apm;day=&day;&amp;area=3</set>
<a href=<use ref>>
&day; <use mese&month;> &year;
</a>
</def><nospace>
<clear>
