create table if not exists metadatatab
(
	id bigint unsigned auto_increment
		primary key,
	parent bigint unsigned null,
	uuidPath text null,
	update_time datetime default CURRENT_TIMESTAMP not null,
	meta_type bigint default 0 null
);

create index Metadata_id_index
	on metadatatab (id);

create index Metadata_parent_index
	on metadatatab (parent);

