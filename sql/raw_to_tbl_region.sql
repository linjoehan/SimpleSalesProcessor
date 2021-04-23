update raw_ref_region set end_date = 'infinity'::timestamp where end_date is null;

insert into ref_region (region,description,start_date,end_date)
select region,description,start_date,end_date from raw_ref_region
on conflict on constraint uq_ref_region_region_start_date do update set end_date = excluded.end_date;

drop table if exists raw_ref_region;
