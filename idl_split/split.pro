pro split

ENVI_SELECT, fid=fid,pos=pos,/FILE_ONLY,dims=dims
if (fid[0] eq -1) then return
print , dims
print , pos

wid = 20
hei = 20

is = 0
while is LE dims[2] do begin
	s0 = is
	s1 = is + wid - 1
	is = is + wid
	if s1 GT dims[2] then break
	if s0+63 GT dims[2] then break
	il = 0
    while il LE dims[4] do begin
    	l0 = il
    	l1 = il + hei - 1
    	il = il + hei
    	if l1 GT dims[4] then break
    	if l0+63 GT dims[4] then break

		img = getImageRGB(fid,s0,l0,wid,hei)
		imgt = getImageRGB(fid,s0,l0,64,64)

		blockname = "e:/nn/blocks/"
		blockname = blockname + strtrim(string(s0),1)+"_"+strtrim(string(l0),1)
		blockname1 = blockname + ".png"
		blockname_thumb = blockname + "_t.png"
		WRITE_PNG, blockname1 ,img, /ORDER
		WRITE_PNG, blockname_thumb ,imgt, /ORDER

    endwhile
	print , is
endwhile

print , 'finished.'
end

FUNCTION getImageRGB , fid , s0,l0,w,h
dims1 = [-1,s0,s0+w-1,l0,l0+h-1]
red =   ENVI_GET_DATA(fid=fid, dims=dims1, pos=2)
green = ENVI_GET_DATA(fid=fid, dims=dims1, pos=1)
blue =  ENVI_GET_DATA(fid=fid, dims=dims1, pos=0)
img = bytarr(3,w,h)
img[0,*,*] = red
img[1,*,*] = green
img[2,*,*] = blue
return , img
end