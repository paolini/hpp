<file input=default.hpp>
<set title>hpp: Istruzioni</set>
<def prg><if not in><p><pre><else><tt></if></def>
<def /prg><if not in></pre><p><else></tt></if></def>
<clear><nospace>
<body languages="it">
Hpp &e; un preprocessore di testi, orientato alla scrittura di documenti HTML.

<h2> Comandi </h2>

La sintassi per i comandi riconosciuti da HPP assomiglia ai TAG
dell'HTML:
<prg>
&lt;TAG <var>name</var>=<var>value</var> <var>[...]</var> &gt;
</prg>

Alcuni comandi prevedono un TAG di chiusura che, come in HTML, &e;
dato dal nome del comando  preceduto dal carattere '/'.
In questo caso la parte di testo compresa tra il TAG di apertura e il
TAG di chiusura viene interpretato dal comando.
All'interno del blocco di testo di un comando ci possono naturalmente
essere "annidiati" altri comandi. 

<h3> Elenco dei comandi </h3>
<ul>
<p><li>
<prg>&lt;NOSPACE&gt;</prg>
Elimina tutti gli spazi (cio&e; spazio, tab e andata a capo) che
seguono il TAG. 

<p><li>
<prg>&lt;IGNORE&gt; <var>testo</var> &lt;/IGNORE&gt; </prg>
Elimina il <prg in><var>testo</var></prg>.

<p><li>
<prg>&lt;LITERAL&gt; <var>testo</var> &lt;/LITERAL&gt; </prg>
Inserisce il <prg in><var>testo</var></prg> senza espanderlo
ulteriormente.

<p><li>
<prg>&lt;SET <var>name</var>=<var>value</var><var>[...]</var>&gt;</prg>
<prg>&lt;SET <var>name</var>&gt;<var> testo </var>&lt;/SET&gt;</prg>
Nella prima forma
setta la variabile <prg in><var>name</var></prg> al valore <prg in><var>value</var></prg>. Nello
stesso TAG possono essere presenti pi&u; di un assegnamento.
Nella seconda forma setta la variabile (ovvero la macro)
<prg in><var>name</var></prg> al valore <prg in><var>testo</var></prg>.

<p><li>
<prg>&lt;DEF <var>name</var>&gt; <var>testo</var> &lt;/DEF&gt;</prg>
Questo comando &e; equivalente al comando <prg in>SET</prg> a parte il
fatto che la macro risultante pu&o; essere usata direttamente con il
comando <prg in>&lt;<var>name ...</var>&gt;</prg> senza dover usare il
costrutto <prg in>&lt;USE <var>name ...</var>&gt;</prg>. Inoltre se
oltre alla macro <prg in><var>name</var></prg> esiste anche la macro <prg
in>/<var>name</var></prg>, quando nel buffer viene trovato il comando
<prg in>&lt;<var>name...</var>&gt;</prg>, viene cercato il tag
corrispondente <prg in>&lt;/<var>name...</var>&gt;</prg>, quindi viene
inserito in un nuovo buffer 
il valore della macro <prg in><var>name</var></prg>, il
testo compreso tra i due TAG e infine il valore della macro
<prg in>/<var>name</var></prg>. Dunque entrambe le macro vengono espanse
nello stesso ambiente, cosicch&e; anche nell'espansione della seconda
macro sono visibili le variabili passate alla prima macro.

<p><li>
<prg>&lt;USE <var>name</var> <var>arg</var>=<var>value</var><var>[...]</var>&gt;</prg>

Copia il valore della variabile (o macro) <prg in><var>name</var></prg> in un nuovo
buffer, e associa al buffer un
nuovo ambiente "figlio" in cui vengono settate le variabili
<prg in><var>arg</var></prg> ai valori <prg in><var>value</var></prg>.
Quindi il testo della macro viene espanso e il buffer risultate viene
rinserito al posto del TAG.

<p><li>
<prg>&lt;IF <var>[</var>NOT<var>]</var> <var>expr</var>&gt; <nospace>
<var>testo1</var> <var>[</var>&lt;ELSE&gt; <var>testo2</var><var>]<nospace>
</var> &lt;/IF&gt;</prg>

Inserisce nel buffer <prg in><var>testo1</var></prg> se <prg in><var>expr</var></prg> &e; vera,
<prg in><var>testo2</var></prg> altrimenti (e se presente). Il testo inserito viene
quindi espanso. Se &e; presente l'opzione <prg in>NOT</prg>
il valore di verit&a; viene invertito.

<prg in><var>expr</var></prg> pu&o; essere: <prg in><var>name</var></prg> oppure <prg
in><var>string1</var>=<var>string2</var></prg>. Nel primo caso
l'espressione &e; vera se la variabile <prg in><var>name</var></prg> &e; stata
definita. Nel secondo caso vengono confrontate le due stringhe.

<p><li>
<prg>&lt;FOREACH <var>name</var> <var>[val ... ]</var>&gt; <nospace>
<var>testo</var> &lt;/FOREACH&gt;
</prg>
Per ogni elemento <prg in><var>val</var></prg> viene creato un nuovo buffer e un
nuovo ambiente nel quale la variabile <prg in><var>name</var></prg> ha valore
<prg in><var>val</var></prg>. Quindi <prg in><var>testo</var></prg> viene espanso e il contenuto
del buffer viene re-inserito al posto del TAG.

<p><li>
<prg>&lt;ENV <var>[GLOBAL]</var>&gt;</prg>

Viene inserita nel buffer una lista della forma <prg
in><var>name</var>=<var>value</var></prg> di tutte le variabili
definite nell'ambiente corrente. Se l'opzione <prg in>GLOBAL</prg> &e;
presente, vengono inserite anche le variabili degli ambienti pi&u;
esterni.

<p><li>
<prg>&lt;MESSAGE <var>[[</var>MSG=<var>]message]</var> <nospace>
<var>[</var>ERR=<var>error]</var>&gt;</prg>
Viene riportato sullo stderr il messaggio <var>message</var> o
l'errore <var>error</var>. Nel caso di un messaggio di errore il
programma termina.

<p><li>
<prg>&lt;FILE [LITERAL] INPUT<var>[</var>=<var>filename]</var>&gt;
&lt;FILE OUTPUT<var>[</var>=<var>filename]</var>&gt;</prg>
Nel primo caso viene inserito nel buffer corrente il contenuto del
file <var>filename</var>. Se <var>filename</var> non viene
specificato, viene letto lo stdin. Se il primo carattere di
<var>filename</var> &e; il carattere '|', viene eseguito
<var>filename</var> e viene inserito il suo output.
In ogni caso il testo appena inserito viene espanso a meno che non sia
presente l'attributo <var>LITERAL</var>.

Nel secondo caso il contenuto del buffer corrente viene salvato sul
file <var>filename</var>. Viene usato stdout o l'input di un programma
in analogia col caso precedente.
Il buffer salvato viene poi cancellato.

<p><li>
<prg>&lt;STAT <var>[</var>PATH BASE EXT MIN MAX
FILE=<var>filename</var> DATE FORMAT=<var>format SIZE DIM]</var> &gt;</prg>

Vengono inserite nel buffer alcune informazioni sul file
<var>filename</var>. Se non viene indicato il nome del file, si
considera il file sorgente <prg in><literal>&SourceFile;</literal></prg>.
Le opzioni <prg in>PATH BASE EXT</prg> specificano quali parti del
nome del file devono essere stampate (rispettivamente il path, il nome
e l'estensione), mentre <prg in>MIN MAX</prg> indicano se l'estensione
deve essere considerata a partire dall'ultimo '.' o dal primo.

L'opzione <prg in>DATE</prg> indica che la data di ultima modifica del
file dev'essere stampata. Se l'opzione <prg in>FILE</prg> non viene
messa, viene stampata la data corrente. L'opzione <prg in
>FORMAT</prg> serve per specificare il formato di visualizzazione di
data e ora, come specificato nelle man-pages del comando <prg
in>strftime</prg>.

L'opzione <prg in>SIZE</prg> stampa la dimensione in bytes del file.

<p><li>
<prg>&lt;CLEAR&gt;</prg>
Tutto il buffer corrente viene cancellato.

<p><li>
<prg>&lt;PROCESS&gt; <var>testo</var> &lt;/PROCESS&gt;</prg>
Viene creato un nuovo buffer con un nuovo ambiente. In questo buffer
viene copiato <var>testo</var> e viene quindi
espanso. 
Se dopo l'espansione il buffer non risulta vuoto, viene generato un
messaggio di avvertimento e comunque il buffer viene distrutto.

</ul>

<h2> Regole di espansione</h2>
L'espansione del testo avviene solamente all'occorrenza di uno dei
caratteri '&lt;', '&amp;' e '$'. 
Nel caso del carattere '&lt;', il preprocessore, dopo aver saltato
eventuali spazi bianchi, legge un nome, cio&e; una sequenza di
caratteri alfanumerici, con in aggiunta i caratteri '/', '_' e '-'.
Se il nome risultante corrisponde ad un comando interno o al nome di
una macro definita con <prg in>DEF</prg>, si procede all'espansione
delle opzioni del comando o della macro, altrimenti il testo viene lasciato cos&i;
com'&e;. 
Le opzioni sono separate da spazi, e sono formate da un nome e
opzionalmente un carattere '=' seguito da una stringa. Le stringhe
sono sequenze di caratteri alfanumerici. Se per&o; in una stringa &e;
presente uno dei caratteri '&lt;','&amp;' o '$', tale carattere viene
espanso a sua volta. Se in una stringa compare un carattere '"', i
successivi caratteri vengono tutti inseriti nella stringa (anche se
non alfanumerici) fino al successivo '"'. Se all'interno di una
stringa "quotata" compare il carattere '\', il carattere successivo
viene inserito senza essere interpretato in alcun modo.
 
<p>
Anche nel caso del carattere '&amp;' il preprocessore legge un nome,
che deve terminare con un carattere ';'. Se il nome corrisponde al
nome di una variabile, il valore della variabile viene inserito
altrimenti il testo non viene modificato.
Il carattere '$' precede anch'esso il nome di una variabile. Il nome
in questo caso pu&o; essere racchiuso tra parentesi graffe. Viene
fatta quindi l'espansione come nel caso di '&amp;'.

<h2>Riga di comando e file di configurazione</h2>
Quando viene eseguito, Hpp crea il buffer principale e l'ambiente
principale. Le opzioni date nella riga di comando vengono quindi
interpretate in due possibili modi: se sono assegnamenti del tipo <prg
in><var>var</var>=<var>value</var></prg>, viene creata nell'ambiente
principale la variabile <prg in><var>var</var></prg> con valore <prg
in><var>value</var></prg>. Le altre opzioni vengono interpretate come
nomi di files, che vengono memorizzati, separati da spazi, nella
variabile <prg in><literal>&SourceFileList;</literal></prg>.
A questo punto viene cercato un file da inserire nel buffer che usando
i comandi del preprocessore si
dovr&a; prendere la responsabilit&a; di caricare in successione i
files indicati nella riga di comando (probabilmente mediante un <prg
in>&lt;FOREACH file <literal>&SourceFileList</literal> &gt;</prg>.
Prima di tutto viene cercato un file chiamato ".hpp" nella directory
corrente, se questo non esiste viene caricato il file "~/.hpp", se
neanche questo esiste viene inserito il seguente buffer di default:
<pre><literal>
&lt;if sourcefilelist&gt;
&lt;foreach SourceFile $sourcefilelist&gt;
&lt;process&gt;
&lt;set OutputFile=&lt;stat base file=$sourceFile&gt;.html&gt;
&lt;message \"Processing SourceFile \"$SourceFile&gt;
&lt;input $SourceFile&gt;
&lt;message \"   ----&gt; \"$OutputFile&gt;
&lt;output $OutputFile&gt;
&lt;/process&gt;
&lt;/foreach&gt;
&lt;else&gt;
&lt;message&gt;
No files specified.
try: hpp &lt;hpp-files&gt;
&lt;/message&gt;
&lt;output&gt;
&lt;/if&gt;
</literal>
</pre>




</body><nospace>
