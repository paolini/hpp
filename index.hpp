<set title>
Hpp: preprocessore ipertestuale
</set>
<file input=default.hpp>
<clear><nospace>
<body>
<ul>
<li> <a href=usage.html>Brevi istruzioni</a>
<li> <a href=hpp-1.0.tgz>Scarica sorgente (formato tgz)</a>
</ul>
<h2> Installazione </h2>
Creare una directory (ad esempio ~/hpp) in cui mettere i sorgenti. Scaricare il file <a href=hpp-1.0.tgz>hpp-1.0.tgz</a> in questa directory. Quindi:
<pre>
cd ~/hpp
tar -xzf hpp-1.0.tgz
</pre>
se quest'ultimo comando non funziona (in quanto l'opzione 'z' per la decompressione non &e; implementata) bisogna invece fare
<pre>
gunzip hpp-1.0.tgz
tar -xf hpp-1.0.tar
</pre>
<p>
A questo punto per compilare il programma digitare
<pre>
make
</pre>
e per installare l'eseguibile in <tt>/usr/local/bin</tt> digitare (come superutente)
<pre>
make install
</pre>
<p>
Il funzionamento del programma non dipende dalla presenza di nessun file a parte ovviamente il programma stesso.

</body>
