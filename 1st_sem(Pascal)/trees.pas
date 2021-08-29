program pr4;
  type t=array[1..512] of char;
       tree=^elem;
       elem=record
         data:string;
         count:integer;
         left,right:tree;
       end;
  const minl=1;
        maxl=6;
  var str:t;
      b:boolean;
      len,first,debugmode,step:integer;
      wd:string;
      list:tree;
      
  procedure input(var str:t;var len:integer);//введение строки
    var p:integer;
        c:char;
  begin
    p:=1;
    read(c);
    if c='.' then writeln('Sequence is empty');
    while c<>'.' do
      begin
        str[p]:=c;
        read(c);
        if c<>'.' then
          p:=p+1;
      end;
    len:=p;
  end;
  
  procedure stringoutput(var str:t; len:integer);//вывод строки
    var i:integer;
  begin
    for i:=1 to len do write(str[i]);
  end;
  
  procedure findword(var str:t;var len,first:integer; var wd:string;var correct:boolean);//выделяет одно конкретное слово
    var i,count:integer;
  begin
    wd:='';//обнуление строки
    i:=first;
    count:=0; //число заглавных латинских букв
    correct:=true;
    while (str[i]<>',') and (i<=len) do
      begin
        wd:=wd+str[i];
        i:=i+1;
        if (str[i]>='A') and (str[i]<='Z') then
          count:=count+1;
      end;
    if (count<minl) or (count>maxl) then
      correct:=false;
    first:=i+1;//иначе перейдем на запятую
  end;
  
  function correctness(var str:t;len:integer):boolean; //костыль
    var first:integer;
        wt:string;
        b:boolean;
  begin
    correctness:=true;
    first:=1;
      while first<=len do //просто считываем все слова и меняем correctness
        begin
          findword(str,len,first,wt,b);
          if not b then
            correctness:=false;
        end;
  end;
          
  procedure createtree(var p:tree); //создание дерева (или поддерева)
  begin
    new(p);
    p^.left:=nil;
    p^.right:=nil;
    p^.count:=0;
    p^.data:='';
  end;
  
  procedure addword(wd:string;var p:tree);
    var r:tree;
  begin
    if p=nil then  // если пришли к пустому дереву или поддереву (например, при добавлении элемента, не встречавшегося ранее
      begin
        createtree(r);
        r^.data:=wd;
        r^.count:=r^.count+1;
        p:=r;
      end
    else
      if p^.data='' then  //для первого слова, когда изначально дерево пустое
        begin
          p^.data:=wd;
          p^.count:=p^.count+1;
        end
      else
        begin
          if wd=p^.data then p^.count:=p^.count+1;
          if wd<p^.data then addword(wd,p^.left);
          if wd>p^.data then addword(wd,p^.right);
        end;
  end;
  
  procedure readtree(p:tree);
    var i:integer;
  begin
    if p<>nil then
      begin
        readtree(p^.left); //самые маленькие
        for i:=1 to p^.count do // средние
          if debugmode=2 then
            writeln(p^.data,'(',p^.count,')')
          else
            write(p^.data,' ');              //нужно сделать, чтобы запятая не выводилась после последнего слова
        readtree(p^.right);  //самые большие
      end;
  end;
  
  procedure deletetree(var t:tree);
    var q:tree;
  begin
    if t<>nil then
      begin
        if (t^.left<>nil) then deletetree(t^.left);
        if (t^.right<>nil then deletetree(t^.right);
        dispose(t);
      end;
  end;
  
begin
  writeln('Chose program mode: 1-standart,2-debug');
  readln(debugmode);
  b:=true;
  input(str,len);
  writeln('Input sequence: ');
    stringoutput(str,len);
    writeln;
  first:=1;
  createtree(list);
  step:=1;
  //if correctness(str,len) then 
    while (first<=len) do
      begin
        findword(str,len,first,wd,b);
        addword(wd,list);
        if debugmode=2 then writeln('Step ',step,' word: ',wd);
        step:=step+1;
      end;
  writeln('Output sequence: ');
  readtree(list);
end.