for WORD in `egrep -o '[A-Za-zА-Яа-я]+' text.txt | sort | uniq -i`
do
   if ! grep -Fixq "$WORD" dic.txt
   then
	printf "$WORD:"   
	tre-agrep -99 -swi $WORD dic.txt | sort -n | head -1
   fi	
done


